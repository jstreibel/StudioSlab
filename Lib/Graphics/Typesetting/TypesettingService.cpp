#include "TypesettingService.h"

#include "Core/Tools/Log.h"
#include "Core/Tools/Resources.h"
#include "Graphics/LaTeX/LaTeX.h"
#include "Utils/EncodingUtils.h"

#include "3rdParty/freetype-gl/freetype-gl.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <memory>
#include <tuple>
#include <unordered_set>

namespace {

    using Slab::Graphics::Typesetting::ETypesetContentKind;
    using Slab::Graphics::Typesetting::ETypesetBackend;
    using Slab::Graphics::Typesetting::FTypesetMetrics;
    using Slab::Graphics::Typesetting::FTypesetRenderable;
    using Slab::Graphics::Typesetting::FTypesetRequest;

    constexpr float DefaultFontPixelSize = 18.0f;

    auto QuantizeScalar(const float value) -> int {
        return static_cast<int>(std::lround(value * 1000.0f));
    }

    auto ResolveDpiScale(const FTypesetRequest &request) -> float {
        return request.Style.DpiScale > 0.0f ? request.Style.DpiScale : 1.0f;
    }

    auto ResolveFontPixelSize(const FTypesetRequest &request) -> float {
        return request.Style.FontPixelSize > 0.0f ? request.Style.FontPixelSize : DefaultFontPixelSize;
    }

    auto ResolveFontFile(const FTypesetRequest &request) -> Slab::Str {
        if (!request.Style.FontFile.empty()) return request.Style.FontFile;
        return Slab::Core::Resources::BuiltinFonts::DejaVuSans();
    }

    auto ResolveStyleToken(const FTypesetRequest &request) -> Slab::Str {
        if (request.Kind == ETypesetContentKind::Text) return ResolveFontFile(request);
        return "NanoTeX.Display";
    }

    auto BlendAlpha(unsigned char dst, unsigned char src) -> unsigned char {
        const auto inv = static_cast<unsigned int>(255u - src);
        return static_cast<unsigned char>(src + (dst * inv + 127u) / 255u);
    }

    auto IsAsciiLetter(const char c) -> bool {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    auto HasBalancedLatexBraces(const Slab::Str &content) -> bool {
        int depth = 0;
        bool escaped = false;

        for (const char c : content) {
            if (escaped) {
                escaped = false;
                continue;
            }

            if (c == '\\') {
                escaped = true;
                continue;
            }

            if (c == '{') {
                ++depth;
                continue;
            }

            if (c == '}') {
                --depth;
                if (depth < 0) return false;
            }
        }

        return depth == 0;
    }

    auto IsKnownNanoTeXCommand(const Slab::Str &command) -> bool {
        static const std::unordered_set<Slab::Str> SupportedCommands = {
            "alpha", "beta", "gamma", "delta", "epsilon", "varepsilon",
            "zeta", "eta", "theta", "vartheta", "iota", "kappa", "lambda",
            "mu", "nu", "xi", "omicron", "pi", "rho", "varrho", "sigma",
            "varsigma", "tau", "upsilon", "phi", "varphi", "chi", "psi",
            "omega", "Gamma", "Delta", "Theta", "Lambda", "Xi", "Pi",
            "Sigma", "Upsilon", "Phi", "Psi", "Omega",
            "mathbb", "mathcal", "mathfrak", "mathrm", "mathit", "mathbf",
            "mathsf", "mathtt", "text",
            "frac", "sqrt", "dot", "ddot", "partial", "nabla", "Box",
            "to", "mapsto", "leftarrow", "rightarrow", "leftrightarrow",
            "in", "notin", "times", "cdot", "pm", "mp", "neq", "leq",
            "geq", "subset", "subseteq", "supset", "supseteq", "cup", "cap",
            "oplus", "otimes", "sum", "prod", "int", "infty", "forall",
            "exists", "emptyset", "ldots", "cdots", "vdots", "ddots",
            "quad", "qquad", "left", "right"
        };

        return SupportedCommands.contains(command);
    }

    auto CanSafelyRouteToNanoTeX(const Slab::Str &content) -> bool {
        if (!HasBalancedLatexBraces(content)) return false;

        for (std::size_t i = 0; i < content.size(); ++i) {
            if (content[i] != '\\') continue;
            if (i + 1 >= content.size()) return false;

            const std::size_t commandStart = i + 1;
            std::size_t commandEnd = commandStart;
            while (commandEnd < content.size() && IsAsciiLetter(content[commandEnd])) {
                ++commandEnd;
            }

            if (commandEnd == commandStart) continue;
            if (!IsKnownNanoTeXCommand(content.substr(commandStart, commandEnd - commandStart))) {
                return false;
            }

            i = commandEnd - 1;
        }

        return true;
    }

    auto MeasureTextGlyphRun(ftgl::texture_font_t *font,
                             const Slab::Str &text,
                             std::vector<std::tuple<ftgl::texture_glyph_t *, int, int>> *placements)
        -> FTypesetRenderable {
        FTypesetRenderable renderable;
        renderable.Backend = ETypesetBackend::FreeTypeText;

        if (font == nullptr) return renderable;

        const int baselinePx = static_cast<int>(std::ceil(font->ascender));
        const int lineHeightPx = std::max(1, static_cast<int>(std::ceil(font->height)));

        if (text.empty()) {
            renderable.PixelWidth = 1;
            renderable.PixelHeight = lineHeightPx;
            renderable.AlphaMask.assign(static_cast<std::size_t>(renderable.PixelWidth * renderable.PixelHeight), 0);
            renderable.Metrics.LineHeight = static_cast<float>(lineHeightPx);
            renderable.Metrics.Height = static_cast<float>(lineHeightPx);
            renderable.Metrics.Baseline = static_cast<float>(baselinePx);
            return renderable;
        }

        float penX = 0.0f;
        int minX = std::numeric_limits<int>::max();
        int minY = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int maxY = std::numeric_limits<int>::min();
        const char *previous = nullptr;

        for (std::size_t i = 0; i < text.size();) {
            const char *codePoint = text.c_str() + i;
            const auto utf8Bytes = Slab::Utf8CharacterByteSize(codePoint);
            const auto step = static_cast<std::size_t>(utf8Bytes > 0 ? utf8Bytes : 1);
            auto *glyph = ftgl::texture_font_get_glyph(font, codePoint);
            if (glyph == nullptr) {
                i += step;
                continue;
            }

            float kerning = 0.0f;
            if (previous != nullptr) kerning = ftgl::texture_glyph_get_kerning(glyph, previous);
            penX += kerning;

            const int x0 = static_cast<int>(std::floor(penX + glyph->offset_x));
            const int y0 = baselinePx - glyph->offset_y;
            const int x1 = x0 + static_cast<int>(glyph->width);
            const int y1 = y0 + static_cast<int>(glyph->height);

            minX = std::min(minX, x0);
            minY = std::min(minY, y0);
            maxX = std::max(maxX, x1);
            maxY = std::max(maxY, y1);

            if (placements != nullptr && glyph->width > 0 && glyph->height > 0) {
                placements->push_back(std::make_tuple(glyph, x0, y0));
            }

            penX += glyph->advance_x;
            previous = codePoint;
            i += step;
        }

        if (minX == std::numeric_limits<int>::max()) {
            minX = 0;
            minY = 0;
            maxX = static_cast<int>(std::ceil(penX));
            maxY = lineHeightPx;
        }

        const int widthPx = std::max(1, std::max(maxX, static_cast<int>(std::ceil(penX))) - minX);
        const int heightPx = std::max(1, std::max(maxY - minY, lineHeightPx));
        const int baselineFromTopPx = baselinePx - minY;

        renderable.PixelWidth = widthPx;
        renderable.PixelHeight = heightPx;
        renderable.AlphaMask.assign(static_cast<std::size_t>(widthPx * heightPx), 0);
        renderable.Metrics.Width = static_cast<float>(widthPx);
        renderable.Metrics.Height = static_cast<float>(heightPx);
        renderable.Metrics.Baseline = static_cast<float>(baselineFromTopPx);
        renderable.Metrics.LineHeight = static_cast<float>(lineHeightPx);

        if (placements == nullptr) return renderable;

        for (auto &[glyph, x0, y0] : *placements) {
            if (glyph == nullptr || font->atlas == nullptr || font->atlas->data == nullptr) continue;

            const int atlasX = static_cast<int>(std::lround(glyph->s0 * static_cast<float>(font->atlas->width)));
            const int atlasY = static_cast<int>(std::lround(glyph->t0 * static_cast<float>(font->atlas->height)));
            const int dstX = x0 - minX;
            const int dstY = y0 - minY;

            for (int row = 0; row < static_cast<int>(glyph->height); ++row) {
                for (int col = 0; col < static_cast<int>(glyph->width); ++col) {
                    const int srcX = atlasX + col;
                    const int srcY = atlasY + row;
                    if (srcX < 0 || srcY < 0 ||
                        srcX >= static_cast<int>(font->atlas->width) ||
                        srcY >= static_cast<int>(font->atlas->height)) {
                        continue;
                    }

                    const int targetX = dstX + col;
                    const int targetY = dstY + row;
                    if (targetX < 0 || targetY < 0 || targetX >= widthPx || targetY >= heightPx) continue;

                    const auto srcIndex = static_cast<std::size_t>(srcY * static_cast<int>(font->atlas->width) + srcX);
                    const auto dstIndex = static_cast<std::size_t>(targetY * widthPx + targetX);
                    renderable.AlphaMask[dstIndex] = BlendAlpha(renderable.AlphaMask[dstIndex], font->atlas->data[srcIndex]);
                }
            }
        }

        return renderable;
    }

} // namespace

namespace Slab::Graphics::Typesetting {

    auto FTypesetCacheKeyHash::operator()(const FTypesetCacheKey &key) const -> std::size_t {
        const auto hashCombine = [](std::size_t seed, std::size_t value) {
            return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6U) + (seed >> 2U));
        };

        std::size_t seed = 0;
        seed = hashCombine(seed, std::hash<int>{}(static_cast<int>(key.Kind)));
        seed = hashCombine(seed, std::hash<Str>{}(key.Content));
        seed = hashCombine(seed, std::hash<Str>{}(key.StyleToken));
        seed = hashCombine(seed, std::hash<int>{}(key.FontPixelSizeMilli));
        seed = hashCombine(seed, std::hash<int>{}(key.DpiScaleMilli));
        return seed;
    }

    FTypesettingService::FTypesettingService() {
        LaTeX::Init();
    }

    FTypesettingService::~FTypesettingService() {
        Clear();

        for (auto &[key, state] : TextFonts) {
            (void) key;
            DestroyFontState(state);
        }
        TextFonts.clear();

        LaTeX::Release();
    }

    auto FTypesettingService::BuildCacheKey(const FTypesetRequest &request) -> FTypesetCacheKey {
        return FTypesetCacheKey{
            .Kind = request.Kind,
            .Content = request.Content,
            .StyleToken = ResolveStyleToken(request),
            .FontPixelSizeMilli = QuantizeScalar(ResolveFontPixelSize(request)),
            .DpiScaleMilli = QuantizeScalar(ResolveDpiScale(request))
        };
    }

    auto FTypesettingService::ResolvePreferredBackend(const FTypesetRequest &request) -> ETypesetBackend {
        return request.Kind == ETypesetContentKind::Math
            ? ETypesetBackend::NanoTeXMath
            : ETypesetBackend::FreeTypeText;
    }

    auto FTypesettingService::Measure(const FTypesetRequest &request) -> FTypesetMetrics {
        return ResolveRenderable(request).Metrics;
    }

    auto FTypesettingService::ResolveRenderable(const FTypesetRequest &request) -> const FTypesetRenderable & {
        return GetOrCreateCacheEntry(request).Renderable;
    }

    auto FTypesettingService::ResolveTexture(const FTypesetRequest &request) -> const FTypesetRenderable & {
        auto &entry = GetOrCreateCacheEntry(request);
        EnsureTextureUploaded(entry.Renderable);
        return entry.Renderable;
    }

    auto FTypesettingService::GetCacheEntryCount() const -> std::size_t {
        return Cache.size();
    }

    auto FTypesettingService::GetMaxCacheEntries() const -> std::size_t {
        return MaxCacheEntries;
    }

    auto FTypesettingService::SetMaxCacheEntries(const std::size_t maxEntries) -> void {
        MaxCacheEntries = std::max<std::size_t>(16, maxEntries);
        TrimCache();
    }

    auto FTypesettingService::TrimCache() -> void {
        while (Cache.size() > MaxCacheEntries) {
            auto oldest = Cache.end();
            for (auto it = Cache.begin(); it != Cache.end(); ++it) {
                if (oldest == Cache.end() || it->second.LastUseSerial < oldest->second.LastUseSerial) {
                    oldest = it;
                }
            }

            if (oldest == Cache.end()) return;
            DestroyTexture(oldest->second.Renderable);
            Cache.erase(oldest);
        }
    }

    auto FTypesettingService::Clear() -> void {
        for (auto &[key, entry] : Cache) {
            (void) key;
            DestroyTexture(entry.Renderable);
        }
        Cache.clear();
    }

    auto FTypesettingService::GetOrCreateCacheEntry(const FTypesetRequest &request) -> FCacheEntry & {
        const auto key = BuildCacheKey(request);
        auto [it, inserted] = Cache.try_emplace(key);
        if (inserted) {
            it->second.Renderable = BuildRenderable(request);
        }

        it->second.LastUseSerial = ++UseSerial;
        TrimCache();
        return it->second;
    }

    auto FTypesettingService::BuildRenderable(const FTypesetRequest &request) -> FTypesetRenderable {
        if (ResolvePreferredBackend(request) == ETypesetBackend::NanoTeXMath) {
            return BuildMathRenderable(request);
        }

        return BuildTextRenderable(request);
    }

    auto FTypesettingService::BuildTextRenderable(const FTypesetRequest &request) -> FTypesetRenderable {
        auto &fontState = GetOrCreateTextFont(request);
        std::vector<std::tuple<ftgl::texture_glyph_t *, int, int>> placements;
        auto renderable = MeasureTextGlyphRun(fontState.Font, request.Content, &placements);

        const auto dpiScale = ResolveDpiScale(request);
        renderable.Metrics.Width /= dpiScale;
        renderable.Metrics.Height /= dpiScale;
        renderable.Metrics.Baseline /= dpiScale;
        renderable.Metrics.LineHeight /= dpiScale;

        return renderable;
    }

    auto FTypesettingService::BuildMathRenderable(const FTypesetRequest &request) -> FTypesetRenderable {
        if (!CanSafelyRouteToNanoTeX(request.Content)) {
            return BuildTextFallbackRenderable(request);
        }

        try {
            const auto dpiScale = ResolveDpiScale(request);
            const auto pixelTextSize = ResolveFontPixelSize(request) * dpiScale;
            std::unique_ptr<tex::TeXRender> render(tex::LaTeX::parse(
                StrToWStr(request.Content),
                4096,
                pixelTextSize,
                pixelTextSize / 3.0f,
                0xff000000));

            if (!render) return BuildTextFallbackRenderable(request);

            const int renderWidthPx = std::max(1, render->getWidth());
            const int renderHeightPx = std::max(1, render->getHeight());
            const int outerPaddingPx = std::max(2, static_cast<int>(std::ceil(pixelTextSize * 0.08f)));
            const int widthPx = renderWidthPx + outerPaddingPx * 2;
            const int heightPx = renderHeightPx + outerPaddingPx * 2;
            const auto surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, widthPx, heightPx);
            const auto context = Cairo::Context::create(surface);
            tex::Graphics2D_cairo graphics2D(context);

            context->set_operator(Cairo::OPERATOR_CLEAR);
            context->paint();
            context->set_operator(Cairo::OPERATOR_OVER);

            render->draw(graphics2D, outerPaddingPx, outerPaddingPx);

            surface->flush();

            FTypesetRenderable renderable;
            renderable.Backend = ETypesetBackend::NanoTeXMath;
            renderable.PixelWidth = widthPx;
            renderable.PixelHeight = heightPx;
            renderable.AlphaMask.assign(static_cast<std::size_t>(widthPx * heightPx), 0);

            const auto *data = surface->get_data();
            const int stride = surface->get_stride();
            for (int y = 0; y < heightPx; ++y) {
                for (int x = 0; x < widthPx; ++x) {
                    const auto srcIndex = static_cast<std::size_t>(y * stride + x * 4);
                    const auto dstIndex = static_cast<std::size_t>(y * widthPx + x);
                    renderable.AlphaMask[dstIndex] = data[srcIndex + 3];
                }
            }

            renderable.Metrics.Width = static_cast<float>(widthPx) / dpiScale;
            renderable.Metrics.Height = static_cast<float>(heightPx) / dpiScale;
            renderable.Metrics.Baseline =
                static_cast<float>(outerPaddingPx + render->getBaseline() * static_cast<float>(renderHeightPx)) / dpiScale;
            renderable.Metrics.LineHeight = renderable.Metrics.Height;

            return renderable;
        } catch (const std::exception &exception) {
            Core::FLog::Warning()
                << "NanoTeX render fallback for '" << request.Content << "': "
                << exception.what()
                << Core::FLog::Flush;
        } catch (...) {
            Core::FLog::Warning()
                << "NanoTeX render fallback for '" << request.Content << "'."
                << Core::FLog::Flush;
        }

        return BuildTextFallbackRenderable(request);
    }

    auto FTypesettingService::BuildTextFallbackRenderable(const FTypesetRequest &request) -> FTypesetRenderable {
        auto fallbackRequest = request;
        fallbackRequest.Kind = ETypesetContentKind::Text;

        auto renderable = BuildTextRenderable(fallbackRequest);
        renderable.Backend = ETypesetBackend::TextFallback;
        return renderable;
    }

    auto FTypesettingService::GetOrCreateTextFont(const FTypesetRequest &request) -> FTextFontState & {
        const auto fontKey = FTextFontKey{
            .FontFile = ResolveFontFile(request),
            .FontPixelSizeMilli = QuantizeScalar(ResolveFontPixelSize(request) * ResolveDpiScale(request))
        };

        auto [it, inserted] = TextFonts.try_emplace(fontKey);
        if (!inserted) return it->second;

        const auto pointSize = static_cast<float>(fontKey.FontPixelSizeMilli) / 1000.0f;
        const auto atlasFactor = std::max(1, static_cast<int>(std::ceil(pointSize / 24.0f)));
        it->second.Atlas = ftgl::texture_atlas_new(
            static_cast<size_t>(atlasFactor * 512),
            static_cast<size_t>(atlasFactor * 512),
            1);
        it->second.Font = ftgl::texture_font_new_from_file(it->second.Atlas, pointSize, fontKey.FontFile.c_str());

        if (it->second.Font == nullptr) {
            throw Exception("Failed to create FreeType font from '" + fontKey.FontFile + "'");
        }

        ftgl::texture_font_load_glyphs(it->second.Font, glyphsToLoad);
        return it->second;
    }

    auto FTypesettingService::EnsureTextureUploaded(FTypesetRenderable &renderable) -> void {
        if (renderable.TextureId != 0 || renderable.AlphaMask.empty()) return;

        std::vector<unsigned char> rgba(
            static_cast<std::size_t>(renderable.PixelWidth * renderable.PixelHeight * 4),
            0);

        for (int y = 0; y < renderable.PixelHeight; ++y) {
            for (int x = 0; x < renderable.PixelWidth; ++x) {
                const auto srcIndex = static_cast<std::size_t>(y * renderable.PixelWidth + x);
                const auto dstIndex = static_cast<std::size_t>((y * renderable.PixelWidth + x) * 4);
                rgba[dstIndex + 0] = 0xff;
                rgba[dstIndex + 1] = 0xff;
                rgba[dstIndex + 2] = 0xff;
                rgba[dstIndex + 3] = renderable.AlphaMask[srcIndex];
            }
        }

        glGenTextures(1, &renderable.TextureId);
        glBindTexture(GL_TEXTURE_2D, renderable.TextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            renderable.PixelWidth,
            renderable.PixelHeight,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            rgba.data());
    }

    auto FTypesettingService::DestroyTexture(FTypesetRenderable &renderable) noexcept -> void {
        if (renderable.TextureId == 0) return;
        glDeleteTextures(1, &renderable.TextureId);
        renderable.TextureId = 0;
    }

    auto FTypesettingService::DestroyFontState(FTextFontState &fontState) noexcept -> void {
        // The existing OpenGL writer keeps font objects alive until process teardown
        // because the bundled freetype-gl font destruction path is unstable here.
        fontState.Font = nullptr;

        if (fontState.Atlas != nullptr) {
            ftgl::texture_atlas_delete(fontState.Atlas);
            fontState.Atlas = nullptr;
        }
    }

} // namespace Slab::Graphics::Typesetting
