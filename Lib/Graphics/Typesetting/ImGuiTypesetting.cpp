#include "ImGuiTypesetting.h"

#include <algorithm>
#include <cstdint>

namespace {

    auto ResolveDpiScale() -> float {
        const auto &io = ImGui::GetIO();
        const float x = io.DisplayFramebufferScale.x > 0.0f ? io.DisplayFramebufferScale.x : 1.0f;
        const float y = io.DisplayFramebufferScale.y > 0.0f ? io.DisplayFramebufferScale.y : 1.0f;
        return std::max(x, y);
    }

    auto NormalizeRequest(Slab::Graphics::Typesetting::FTypesetRequest request,
                          Slab::Graphics::Typesetting::FTypesetStyle styleOverride)
        -> Slab::Graphics::Typesetting::FTypesetRequest {
        if (styleOverride.FontPixelSize > 0.0f) request.Style.FontPixelSize = styleOverride.FontPixelSize;
        if (styleOverride.DpiScale > 0.0f) request.Style.DpiScale = styleOverride.DpiScale;
        if (!styleOverride.FontFile.empty()) request.Style.FontFile = std::move(styleOverride.FontFile);

        if (request.Style.FontPixelSize <= 0.0f) request.Style.FontPixelSize = ImGui::GetFontSize();
        if (request.Style.DpiScale <= 0.0f) request.Style.DpiScale = ResolveDpiScale();

        return request;
    }

    auto DrawRenderable(const Slab::Graphics::Typesetting::FTypesetRequest &request,
                        const Slab::Graphics::Typesetting::FTypesetRenderable &renderable,
                        const ImVec2 min,
                        const ImU32 tint) -> void {
        auto *drawList = ImGui::GetWindowDrawList();
        const ImVec2 max = ImVec2(min.x + renderable.Metrics.Width, min.y + renderable.Metrics.Height);

        if (renderable.TextureId != 0) {
            drawList->AddImage(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(renderable.TextureId)), min, max, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), tint);
            return;
        }

        drawList->AddText(min, tint, request.Content.c_str());
    }

    auto ResolveTint(const ImU32 explicitTint, const ImGuiCol fallbackColor, const float alphaMultiplier = 1.0f) -> ImU32 {
        if (explicitTint != 0) return explicitTint;

        auto color = ImGui::GetStyleColorVec4(fallbackColor);
        color.w *= alphaMultiplier;
        return ImGui::GetColorU32(color);
    }

    auto DrawSelectableRowChrome(const ImVec2 rectMin,
                                 const ImVec2 rectMax,
                                 const Slab::Graphics::Typesetting::FImGuiSelectableTypesetRowOptions &options,
                                 const bool bHovered) -> void {
        auto *drawList = ImGui::GetWindowDrawList();
        const float rounding = std::max(0.0f, options.CornerRounding);

        ImU32 fillTint = options.BackgroundTint;
        if (options.bSelected) {
            fillTint = ResolveTint(options.SelectedBackgroundTint, ImGuiCol_Header, 0.95f);
        } else if (bHovered) {
            fillTint = ResolveTint(options.HoveredBackgroundTint, ImGuiCol_HeaderHovered, 0.55f);
        }

        if (fillTint != 0) {
            drawList->AddRectFilled(rectMin, rectMax, fillTint, rounding);
        }

        const float accentWidth = std::max(0.0f, options.AccentWidth);
        ImU32 accentTint = options.AccentTint;
        if (options.bSelected) {
            accentTint = ResolveTint(options.SelectedAccentTint, ImGuiCol_CheckMark, 1.0f);
        } else if (bHovered) {
            accentTint = ResolveTint(options.HoveredAccentTint, ImGuiCol_CheckMark, 0.8f);
        }

        if (accentTint != 0 && accentWidth > 0.0f) {
            const ImVec2 accentMax(
                std::min(rectMin.x + accentWidth, rectMax.x),
                rectMax.y);
            drawList->AddRectFilled(rectMin, accentMax, accentTint, rounding, ImDrawFlags_RoundCornersLeft);
        }

        const float borderThickness = std::max(0.0f, options.BorderThickness);
        if (borderThickness <= 0.0f) return;

        ImU32 borderTint = options.BorderTint;
        if (options.bSelected) {
            borderTint = ResolveTint(options.SelectedBorderTint, ImGuiCol_BorderShadow, 0.95f);
        } else if (bHovered) {
            borderTint = ResolveTint(options.HoveredBorderTint, ImGuiCol_Border, 0.85f);
        }

        if (borderTint != 0) {
            drawList->AddRect(rectMin, rectMax, borderTint, rounding, 0, borderThickness);
        }
    }

} // namespace

namespace Slab::Graphics::Typesetting {

    auto ImGuiMeasureTypeset(FTypesettingService &service,
                             const FTypesetRequest &request,
                             FTypesetStyle style) -> ImVec2 {
        const auto normalizedRequest = NormalizeRequest(request, std::move(style));
        const auto metrics = service.Measure(normalizedRequest);
        return ImVec2(metrics.Width, metrics.Height);
    }

    auto ImGuiTypeset(FTypesettingService &service,
                      const FTypesetRequest &request,
                      const FImGuiTypesetDrawOptions &options) -> FImGuiTypesetResult {
        const auto normalizedRequest = NormalizeRequest(request, options.Style);
        const auto &renderable = service.ResolveTexture(normalizedRequest);

        const ImVec2 totalSize(
            renderable.Metrics.Width + options.Padding.x * 2.0f,
            renderable.Metrics.Height + options.Padding.y * 2.0f);

        ImGui::Dummy(totalSize);

        const ImVec2 rectMin = ImGui::GetItemRectMin();
        const ImVec2 drawMin(rectMin.x + options.Padding.x, rectMin.y + options.Padding.y);

        DrawRenderable(normalizedRequest, renderable, drawMin, options.Tint);

        return FImGuiTypesetResult{
            .Size = totalSize,
            .RectMin = rectMin,
            .RectMax = ImGui::GetItemRectMax(),
            .Baseline = renderable.Metrics.Baseline + options.Padding.y,
            .bPressed = false,
            .bHovered = ImGui::IsItemHovered()
        };
    }

    auto ImGuiTypesetText(FTypesettingService &service,
                          const Str &text,
                          const FImGuiTypesetDrawOptions &options) -> FImGuiTypesetResult {
        return ImGuiTypeset(service, MakeTextRequest(text), options);
    }

    auto ImGuiTypesetMath(FTypesettingService &service,
                          const Str &text,
                          const FImGuiTypesetDrawOptions &options) -> FImGuiTypesetResult {
        return ImGuiTypeset(service, MakeMathRequest(text), options);
    }

    auto ImGuiTypesetSelectableRow(FTypesettingService &service,
                                   const char *id,
                                   const FTypesetRequest &primaryRequest,
                                   const FImGuiSelectableTypesetRowOptions &options) -> FImGuiTypesetResult {
        const auto normalizedPrimary = NormalizeRequest(primaryRequest, primaryRequest.Style);
        const auto primaryMetrics = service.Measure(normalizedPrimary);

        FTypesetMetrics secondaryMetrics;
        FTypesetRequest secondaryRequest;
        const bool bHasSecondary = !options.SecondaryText.empty();
        if (bHasSecondary) {
            secondaryRequest = NormalizeRequest(MakeTextRequest(options.SecondaryText, options.SecondaryStyle), options.SecondaryStyle);
            secondaryMetrics = service.Measure(secondaryRequest);
        }

        const float rowHeight =
            options.Padding.y * 2.0f +
            primaryMetrics.Height +
            (bHasSecondary ? options.InnerSpacing + secondaryMetrics.Height : 0.0f);
        const float rowWidth = std::max(ImGui::GetContentRegionAvail().x, 1.0f);

        const bool bPressed = ImGui::Selectable(id, options.bSelected, 0, ImVec2(rowWidth, rowHeight));
        const ImVec2 rectMin = ImGui::GetItemRectMin();
        const ImVec2 rectMax = ImGui::GetItemRectMax();
        const ImVec2 contentMin(rectMin.x + options.Padding.x, rectMin.y + options.Padding.y);
        const bool bHovered = ImGui::IsItemHovered();

        DrawSelectableRowChrome(rectMin, rectMax, options, bHovered);

        auto *drawList = ImGui::GetWindowDrawList();
        drawList->PushClipRect(rectMin, rectMax, true);

        const auto &primaryRenderable = service.ResolveTexture(normalizedPrimary);
        DrawRenderable(normalizedPrimary, primaryRenderable, contentMin, options.PrimaryTint);

        if (bHasSecondary) {
            const ImVec2 secondaryPosition(
                contentMin.x,
                contentMin.y + primaryMetrics.Height + options.InnerSpacing);
            const auto &secondaryRenderable = service.ResolveTexture(secondaryRequest);
            DrawRenderable(secondaryRequest, secondaryRenderable, secondaryPosition, options.SecondaryTint);
        }

        drawList->PopClipRect();

        return FImGuiTypesetResult{
            .Size = ImVec2(rowWidth, rowHeight),
            .RectMin = rectMin,
            .RectMax = rectMax,
            .Baseline = options.Padding.y + primaryMetrics.Baseline,
            .bPressed = bPressed,
            .bHovered = bHovered
        };
    }

    auto ImGuiTypesetLabelValue(FTypesettingService &service,
                                const Str &label,
                                const FTypesetRequest &valueRequest,
                                const FImGuiTypesetDrawOptions &labelOptions,
                                const FImGuiTypesetDrawOptions &valueOptions) -> void {
        ImGui::BeginGroup();
        ImGuiTypesetText(service, label, labelOptions);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGuiTypeset(service, valueRequest, valueOptions);
        ImGui::EndGroup();
    }

} // namespace Slab::Graphics::Typesetting
