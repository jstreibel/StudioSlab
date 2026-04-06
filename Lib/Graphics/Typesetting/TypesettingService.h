#ifndef STUDIOSLAB_TYPESETTING_SERVICE_H
#define STUDIOSLAB_TYPESETTING_SERVICE_H

#include "Graphics/OpenGL/OpenGL.h"
#include "Utils/String.h"

#include <cstdint>
#include <unordered_map>
#include <map>
#include <vector>
#include <compare>

namespace ftgl {
    struct texture_atlas_t;
    struct texture_font_t;
}

namespace Slab::Graphics::Typesetting {

    enum class ETypesetContentKind : unsigned char {
        Text = 0,
        Math
    };

    enum class ETypesetBackend : unsigned char {
        FreeTypeText = 0,
        NanoTeXMath,
        TextFallback
    };

    struct FTypesetStyle {
        float FontPixelSize = 0.0f;
        float DpiScale = 1.0f;
        Str FontFile;
    };

    struct FTypesetRequest {
        ETypesetContentKind Kind = ETypesetContentKind::Text;
        Str Content;
        FTypesetStyle Style;
    };

    struct FTypesetMetrics {
        float Width = 0.0f;
        float Height = 0.0f;
        float Baseline = 0.0f;
        float LineHeight = 0.0f;
    };

    struct FTypesetRenderable {
        ETypesetBackend Backend = ETypesetBackend::FreeTypeText;
        FTypesetMetrics Metrics;
        int PixelWidth = 0;
        int PixelHeight = 0;
        std::vector<unsigned char> AlphaMask;
        GLuint TextureId = 0;

        [[nodiscard]] auto HasTexture() const -> bool { return TextureId != 0; }
        [[nodiscard]] auto HasPixels() const -> bool { return !AlphaMask.empty(); }
    };

    struct FTypesetCacheKey {
        ETypesetContentKind Kind = ETypesetContentKind::Text;
        Str Content;
        Str StyleToken;
        int FontPixelSizeMilli = 0;
        int DpiScaleMilli = 1000;

        auto operator==(const FTypesetCacheKey &) const -> bool = default;
    };

    struct FTypesetCacheKeyHash {
        auto operator()(const FTypesetCacheKey &key) const -> std::size_t;
    };

    class FTypesettingService {
    public:
        FTypesettingService();
        ~FTypesettingService();

        FTypesettingService(const FTypesettingService &) = delete;
        auto operator=(const FTypesettingService &) -> FTypesettingService & = delete;

        static auto BuildCacheKey(const FTypesetRequest &request) -> FTypesetCacheKey;
        static auto ResolvePreferredBackend(const FTypesetRequest &request) -> ETypesetBackend;

        auto Measure(const FTypesetRequest &request) -> FTypesetMetrics;
        auto ResolveRenderable(const FTypesetRequest &request) -> const FTypesetRenderable &;
        auto ResolveTexture(const FTypesetRequest &request) -> const FTypesetRenderable &;

        auto GetCacheEntryCount() const -> std::size_t;
        auto GetMaxCacheEntries() const -> std::size_t;
        auto SetMaxCacheEntries(std::size_t maxEntries) -> void;
        auto TrimCache() -> void;
        auto Clear() -> void;

    private:
        struct FCacheEntry {
            FTypesetRenderable Renderable;
            std::uint64_t LastUseSerial = 0;
        };

        struct FTextFontKey {
            Str FontFile;
            int FontPixelSizeMilli = 0;

            auto operator<=>(const FTextFontKey &) const = default;
        };

        struct FTextFontState {
            ftgl::texture_atlas_t *Atlas = nullptr;
            ftgl::texture_font_t *Font = nullptr;
        };

        std::unordered_map<FTypesetCacheKey, FCacheEntry, FTypesetCacheKeyHash> Cache;
        std::map<FTextFontKey, FTextFontState> TextFonts;
        std::uint64_t UseSerial = 0;
        std::size_t MaxCacheEntries = 256;

        auto GetOrCreateCacheEntry(const FTypesetRequest &request) -> FCacheEntry &;
        auto BuildRenderable(const FTypesetRequest &request) -> FTypesetRenderable;
        auto BuildTextRenderable(const FTypesetRequest &request) -> FTypesetRenderable;
        auto BuildMathRenderable(const FTypesetRequest &request) -> FTypesetRenderable;
        auto BuildTextFallbackRenderable(const FTypesetRequest &request) -> FTypesetRenderable;
        auto GetOrCreateTextFont(const FTypesetRequest &request) -> FTextFontState &;
        auto EnsureTextureUploaded(FTypesetRenderable &renderable) -> void;
        auto DestroyTexture(FTypesetRenderable &renderable) noexcept -> void;
        auto DestroyFontState(FTextFontState &fontState) noexcept -> void;
    };

    inline auto MakeTextRequest(Str content, FTypesetStyle style = {}) -> FTypesetRequest {
        return FTypesetRequest{ETypesetContentKind::Text, std::move(content), std::move(style)};
    }

    inline auto MakeMathRequest(Str content, FTypesetStyle style = {}) -> FTypesetRequest {
        return FTypesetRequest{ETypesetContentKind::Math, std::move(content), std::move(style)};
    }

} // namespace Slab::Graphics::Typesetting

#endif // STUDIOSLAB_TYPESETTING_SERVICE_H
