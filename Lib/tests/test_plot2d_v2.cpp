#include <catch2/catch_all.hpp>
#include <cmath>

#include "Graphics/Plot2D/V2/Plot2DV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"

namespace {

    using namespace Slab;
    using namespace Slab::Graphics::Plot2D::V2;
    using Slab::Graphics::FPoint2D;
    using Slab::Graphics::PlotStyle;

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    class FSin2XFunction final : public Slab::Math::RtoR::Function {
    public:
        auto operator()(const DevFloat x) const -> DevFloat override {
            return std::sin(2.0 * x);
        }

        auto Clone() const -> TPointer<Type> override {
            return New<FSin2XFunction>(*this);
        }
    };

    auto CountPolylineCommands(const FPlotDrawListV2 &drawList) -> int {
        int count = 0;
        for (const auto &command : drawList.GetCommands()) {
            if (std::holds_alternative<FPolylineCommandV2>(command)) {
                ++count;
            }
        }

        return count;
    }

} // namespace

TEST_CASE("Plot2D V2 emits draw commands through backend abstraction", "[Plot2DV2]") {
    FPlot2DWindowV2 window("Render Test", {-2.0, 2.0, -2.0, 2.0}, {0, 800, 0, 600});

    auto axis = New<FAxisArtistV2>();
    auto points = New<FPointSetArtistV2>(Vector<FPoint2D>{{-1.0, -0.5}, {0.0, 1.0}, {1.5, 0.25}});
    points->SetLabel("samples");

    auto function = New<FRtoRFunctionArtistV2>(
        New<FSin2XFunction>(),
        PlotStyle(Slab::Graphics::FlatBlue, Slab::Graphics::LineStrip, false, Slab::Graphics::Nil, 1.5f),
        128,
        -2.0,
        2.0);
    function->SetLabel("sin(2x)");

    window.AddArtist(axis, -10);
    window.AddArtist(points, 0);
    window.AddArtist(function, 1);

    FRecordingRenderBackendV2 backend;
    const auto rendered = window.Render(backend);

    REQUIRE(rendered);
    REQUIRE(backend.GetRenderCallCount() == 1);

    const auto &drawList = backend.GetLastDrawList();
    REQUIRE(drawList.GetCommands().size() >= 3);
    REQUIRE(CountPolylineCommands(drawList) >= 2);
    REQUIRE(drawList.GetLegendEntries().size() == 2);

    const auto &frame = backend.GetLastFrame();
    REQUIRE(frame.WindowId == window.GetWindowId());
    REQUIRE(frame.Title == window.GetTitle());
}

TEST_CASE("Plot2D V2 fits region to artist bounds", "[Plot2DV2]") {
    FPlot2DWindowV2 window("Bounds Test", {-1.0, 1.0, -1.0, 1.0});

    auto points = New<FPointSetArtistV2>(Vector<FPoint2D>{{-3.0, 7.5}, {4.0, -2.0}, {6.0, 1.0}});
    points->SetAffectGraphRanges(true);
    window.AddArtist(points);

    const bool fitted = window.FitRegionToArtists(0.0);

    REQUIRE(fitted);
    REQUIRE(window.GetRegion().xMin == Catch::Approx(-3.0));
    REQUIRE(window.GetRegion().xMax == Catch::Approx(6.0));
    REQUIRE(window.GetRegion().yMin == Catch::Approx(-2.0));
    REQUIRE(window.GetRegion().yMax == Catch::Approx(7.5));
}

TEST_CASE("Plot2D V2 reflection catalog supports query and set", "[Plot2DV2][ReflectionV2]") {
    FPlot2DWindowV2 window("Reflection Test", {-1.0, 1.0, -1.0, 1.0});

    auto points = New<FPointSetArtistV2>(Vector<FPoint2D>{{-2.0, -1.0}, {2.0, 3.0}});
    points->SetLabel("reflect-points");
    window.AddArtist(points);

    FPlotReflectionCatalogV2 catalog;
    catalog.RefreshFromLiveWindows();

    const auto windowInterfaceId = Str("v2.plot.window.") + window.GetWindowId();
    const auto artistInterfaceId = Str("v2.plot.artist.") + window.GetWindowId() + "." + points->GetArtistId();

    REQUIRE(catalog.GetInterface(windowInterfaceId) != nullptr);
    REQUIRE(catalog.GetInterface(artistInterfaceId) != nullptr);

    const ReflectionV2::FInvocationContextV2 context{
        .CurrentThread = ReflectionV2::EThreadAffinity::Any,
        .bRuntimeRunning = false
    };

    ReflectionV2::FValueMapV2 getInputs;
    getInputs["parameter_id"] = ReflectionV2::MakeStringValue("x_min");

    const auto getBefore = catalog.Invoke(
        windowInterfaceId,
        CPlotOperationIdQueryGetParameterV2,
        getInputs,
        context);

    REQUIRE(getBefore.IsOk());
    REQUIRE(std::stod(getBefore.OutputMap.at("value").Encoded) == Catch::Approx(-1.0));

    ReflectionV2::FValueMapV2 setInputs;
    setInputs["parameter_id"] = ReflectionV2::MakeStringValue("x_min");
    setInputs["value"] = ReflectionV2::MakeEncodedValue(ReflectionV2::CTypeIdScalarFloat64, "-3.5");

    const auto setResult = catalog.Invoke(
        windowInterfaceId,
        CPlotOperationIdCommandSetParameterV2,
        setInputs,
        context);

    REQUIRE(setResult.IsOk());
    REQUIRE(window.GetRegion().xMin == Catch::Approx(-3.5));

    ReflectionV2::FValueMapV2 setArtistVisibleInputs;
    setArtistVisibleInputs["parameter_id"] = ReflectionV2::MakeStringValue("visible");
    setArtistVisibleInputs["value"] = ReflectionV2::MakeEncodedValue(ReflectionV2::CTypeIdScalarBool, "false");

    const auto setArtistVisibleResult = catalog.Invoke(
        artistInterfaceId,
        CPlotOperationIdCommandSetParameterV2,
        setArtistVisibleInputs,
        context);

    REQUIRE(setArtistVisibleResult.IsOk());
    REQUIRE_FALSE(points->IsVisible());

    const auto fitResult = catalog.Invoke(
        windowInterfaceId,
        "command.window.fit_to_artists",
        {},
        context);

    REQUIRE(fitResult.IsOk());
    REQUIRE(fitResult.OutputMap.at("fitted").Encoded == "true");
}
