#include <catch2/catch_all.hpp>

#include "Core/Tools/Resources.h"
#include "Graphics/Typesetting/TypesettingService.h"

namespace {

    auto MakeScientificTextRequest() -> Slab::Graphics::Typesetting::FTypesetRequest {
        Slab::Graphics::Typesetting::FTypesetStyle style;
        style.FontPixelSize = 18.0f;
        style.DpiScale = 1.0f;
        style.FontFile = Slab::Core::Resources::BuiltinFonts::DejaVuSans();
        return Slab::Graphics::Typesetting::MakeTextRequest("t ∈ ℝ", style);
    }

    auto MakeMathFormulaRequest() -> Slab::Graphics::Typesetting::FTypesetRequest {
        Slab::Graphics::Typesetting::FTypesetStyle style;
        style.FontPixelSize = 18.0f;
        style.DpiScale = 1.0f;
        return Slab::Graphics::Typesetting::MakeMathRequest("\\ddot x + \\omega^2 x = 0", style);
    }

} // namespace

TEST_CASE("Typesetting cache key captures kind content style and DPI", "[Typesetting]") {
    using namespace Slab::Graphics::Typesetting;

    auto base = MakeScientificTextRequest();
    const auto baseKey = FTypesettingService::BuildCacheKey(base);

    auto differentContent = base;
    differentContent.Content = "x(t) : ℝ → ℝ";
    CHECK(baseKey != FTypesettingService::BuildCacheKey(differentContent));

    auto differentKind = base;
    differentKind.Kind = ETypesetContentKind::Math;
    CHECK(baseKey != FTypesettingService::BuildCacheKey(differentKind));

    auto differentFont = base;
    differentFont.Style.FontFile = Slab::Core::Resources::BuiltinFonts::JuliaMono_Regular();
    CHECK(baseKey != FTypesettingService::BuildCacheKey(differentFont));

    auto differentSize = base;
    differentSize.Style.FontPixelSize = 22.0f;
    CHECK(baseKey != FTypesettingService::BuildCacheKey(differentSize));

    auto differentDpi = base;
    differentDpi.Style.DpiScale = 1.5f;
    CHECK(baseKey != FTypesettingService::BuildCacheKey(differentDpi));
}

TEST_CASE("Typesetting service routes text and math through the expected backends", "[Typesetting]") {
    using namespace Slab::Graphics::Typesetting;

    CHECK(FTypesettingService::ResolvePreferredBackend(MakeScientificTextRequest()) == ETypesetBackend::FreeTypeText);
    CHECK(FTypesettingService::ResolvePreferredBackend(MakeMathFormulaRequest()) == ETypesetBackend::NanoTeXMath);
}

TEST_CASE("Typesetting service measures scientific text and math without GL upload", "[Typesetting]") {
    using namespace Slab::Graphics::Typesetting;

    FTypesettingService service;

    const auto textMetrics = service.Measure(MakeScientificTextRequest());
    CHECK(textMetrics.Width > 0.0f);
    CHECK(textMetrics.Height > 0.0f);
    CHECK(textMetrics.Baseline > 0.0f);
    CHECK(textMetrics.Baseline < textMetrics.Height);
    CHECK(textMetrics.LineHeight > 0.0f);

    const auto mathMetrics = service.Measure(MakeMathFormulaRequest());
    CHECK(mathMetrics.Width > 0.0f);
    CHECK(mathMetrics.Height > 0.0f);
    CHECK(mathMetrics.Baseline > 0.0f);
    CHECK(mathMetrics.Baseline < mathMetrics.Height);

    REQUIRE(service.GetCacheEntryCount() == 2);
}

TEST_CASE("Typesetting service caches repeated requests and falls back cleanly on bad math", "[Typesetting]") {
    using namespace Slab::Graphics::Typesetting;

    FTypesettingService service;

    const auto textRequest = MakeScientificTextRequest();
    const auto &firstText = service.ResolveRenderable(textRequest);
    const auto &secondText = service.ResolveRenderable(textRequest);
    CHECK(service.GetCacheEntryCount() == 1);
    CHECK(firstText.Backend == ETypesetBackend::FreeTypeText);
    CHECK(secondText.Metrics.Width == Catch::Approx(firstText.Metrics.Width));

    FTypesetStyle invalidMathStyle;
    invalidMathStyle.FontPixelSize = 18.0f;
    invalidMathStyle.DpiScale = 1.0f;
    const auto invalidMathRequest = MakeMathRequest("\\definitelyUnknownMacro{x}", invalidMathStyle);
    const auto &invalidMath = service.ResolveRenderable(invalidMathRequest);
    CHECK(service.GetCacheEntryCount() == 2);
    CHECK(invalidMath.Metrics.Width > 0.0f);
    CHECK(invalidMath.Backend == ETypesetBackend::TextFallback);
}
