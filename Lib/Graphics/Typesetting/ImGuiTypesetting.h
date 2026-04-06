#ifndef STUDIOSLAB_IMGUI_TYPESETTING_H
#define STUDIOSLAB_IMGUI_TYPESETTING_H

#include "3rdParty/ImGui.h"
#include "TypesettingService.h"

namespace Slab::Graphics::Typesetting {

    struct FImGuiTypesetDrawOptions {
        FTypesetStyle Style;
        ImVec2 Padding = ImVec2(0.0f, 0.0f);
        ImU32 Tint = IM_COL32(255, 255, 255, 255);
    };

    struct FImGuiSelectableTypesetRowOptions {
        bool bSelected = false;
        Str SecondaryText;
        FTypesetStyle SecondaryStyle;
        ImVec2 Padding = ImVec2(8.0f, 6.0f);
        float InnerSpacing = 4.0f;
        float CornerRounding = 6.0f;
        float BorderThickness = 1.0f;
        float AccentWidth = 4.0f;
        ImU32 PrimaryTint = IM_COL32(255, 255, 255, 255);
        ImU32 SecondaryTint = IM_COL32(180, 180, 180, 255);
        ImU32 BackgroundTint = 0;
        ImU32 HoveredBackgroundTint = 0;
        ImU32 SelectedBackgroundTint = 0;
        ImU32 BorderTint = 0;
        ImU32 HoveredBorderTint = 0;
        ImU32 SelectedBorderTint = 0;
        ImU32 AccentTint = 0;
        ImU32 HoveredAccentTint = 0;
        ImU32 SelectedAccentTint = 0;
    };

    struct FImGuiTypesetResult {
        ImVec2 Size = ImVec2(0.0f, 0.0f);
        ImVec2 RectMin = ImVec2(0.0f, 0.0f);
        ImVec2 RectMax = ImVec2(0.0f, 0.0f);
        float Baseline = 0.0f;
        bool bPressed = false;
        bool bHovered = false;
    };

    auto ImGuiMeasureTypeset(FTypesettingService &service,
                             const FTypesetRequest &request,
                             FTypesetStyle style = {}) -> ImVec2;

    auto ImGuiTypeset(FTypesettingService &service,
                      const FTypesetRequest &request,
                      const FImGuiTypesetDrawOptions &options = {}) -> FImGuiTypesetResult;

    auto ImGuiTypesetText(FTypesettingService &service,
                          const Str &text,
                          const FImGuiTypesetDrawOptions &options = {}) -> FImGuiTypesetResult;

    auto ImGuiTypesetMath(FTypesettingService &service,
                          const Str &text,
                          const FImGuiTypesetDrawOptions &options = {}) -> FImGuiTypesetResult;

    auto ImGuiTypesetSelectableRow(FTypesettingService &service,
                                   const char *id,
                                   const FTypesetRequest &primaryRequest,
                                   const FImGuiSelectableTypesetRowOptions &options = {}) -> FImGuiTypesetResult;

    auto ImGuiTypesetLabelValue(FTypesettingService &service,
                                const Str &label,
                                const FTypesetRequest &valueRequest,
                                const FImGuiTypesetDrawOptions &labelOptions = {},
                                const FImGuiTypesetDrawOptions &valueOptions = {}) -> void;

} // namespace Slab::Graphics::Typesetting

#endif // STUDIOSLAB_IMGUI_TYPESETTING_H
