#include "Plot2DWindowHostV2.h"

#include "3rdParty/ImGui.h"
#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Core/SlabCore.h"
#include "Graphics/Plot2D/V2/Backends/OpenGLRenderBackendV2.h"
#include "Graphics/Modules/Animator/Animator.h"
#include "Graphics/OpenGL/Utils.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    auto FPlot2DWindowHostV2::DistanceSquared(const Slab::Graphics::FPoint2D &lhs,
                                             const Slab::Graphics::FPoint2D &rhs) -> Slab::DevFloat {
        const auto dx = lhs.x - rhs.x;
        const auto dy = lhs.y - rhs.y;
        return (dx * dx) + (dy * dy);
    }

    auto FPlot2DWindowHostV2::MakeOverlayId(const char *suffix) const -> Str {
        return Str(suffix) + "##" + PlotWindowId;
    }

    namespace {

        constexpr float COverlayMarginV2 = 14.0f;
        constexpr float COverlayStripPaddingV2 = 8.0f;
        constexpr float COverlayStripSpacingV2 = 10.0f;
        constexpr float CBaseToolbarButtonSizeV2 = 30.0f;
        constexpr float CBaseRightButtonSizeV2 = 34.0f;
        constexpr float CToolbarButtonScaleV2 = 3.0f;
        constexpr float CRightButtonScaleV2 = 3.0f;
        constexpr float CDetailWidthMinV2 = 310.0f;
        constexpr float CDetailWidthMaxV2 = 460.0f;
        constexpr float CToolbarBackgroundAlphaV2 = 0.0f;
        constexpr float CRightBackgroundAlphaV2 = 0.0f;
        constexpr bool CTransparentBackgroundV2 = true;
        constexpr bool CRightStripAlignTopV2 = true;
        constexpr float CRightStripTopOffsetV2 = 8.0f;
        constexpr float CHudChromeGapV2 = 10.0f;
        constexpr std::size_t CToolbarButtonCountV2 = 1;
        constexpr std::size_t CRightButtonCountV2 = 6;

        struct FOverlayIconButtonV2 {
            Str Id;
            std::function<Str()> GetIconLabel;
            Str Tooltip;
            std::function<void()> OnPressed;
            std::function<bool()> IsActive;
        };

        struct FOverlayChromeLayoutV2 {
            float ToolbarButtonSize = 0.0f;
            float RightButtonSize = 0.0f;
            ImVec2 ToolbarPos = {};
            ImVec2 ToolbarSize = {};
            ImVec2 RightPos = {};
            ImVec2 RightSize = {};
            bool bHasDetailPanel = false;
            ImVec2 DetailPos = {};
            ImVec2 DetailSize = {};
        };

        struct FFloatingStripPaletteV2 {
            ImVec4 Button = {};
            ImVec4 ButtonHovered = {};
            ImVec4 ButtonActive = {};
            ImVec4 ActiveButton = {};
            ImVec4 ActiveButtonHovered = {};
            ImVec4 Text = {};
            ImVec4 TextDisabled = {};
            ImVec4 PopupBg = {};
        };

        auto EncodeUtf8CodePointV2(const unsigned int codePoint) -> Str {
            Str encoded;

            if (codePoint <= 0x7F) {
                encoded.push_back(static_cast<char>(codePoint));
            } else if (codePoint <= 0x7FF) {
                encoded.push_back(static_cast<char>(0xC0u | ((codePoint >> 6u) & 0x1Fu)));
                encoded.push_back(static_cast<char>(0x80u | (codePoint & 0x3Fu)));
            } else if (codePoint <= 0xFFFF) {
                encoded.push_back(static_cast<char>(0xE0u | ((codePoint >> 12u) & 0x0Fu)));
                encoded.push_back(static_cast<char>(0x80u | ((codePoint >> 6u) & 0x3Fu)));
                encoded.push_back(static_cast<char>(0x80u | (codePoint & 0x3Fu)));
            } else if (codePoint <= 0x10FFFF) {
                encoded.push_back(static_cast<char>(0xF0u | ((codePoint >> 18u) & 0x07u)));
                encoded.push_back(static_cast<char>(0x80u | ((codePoint >> 12u) & 0x3Fu)));
                encoded.push_back(static_cast<char>(0x80u | ((codePoint >> 6u) & 0x3Fu)));
                encoded.push_back(static_cast<char>(0x80u | (codePoint & 0x3Fu)));
            } else {
                encoded = "?";
            }

            return encoded;
        }

        namespace FontAwesomeSolidIconsV2 {
            const Str Fit = EncodeUtf8CodePointV2(0xF065);
            const Str AutoAdjust = EncodeUtf8CodePointV2(0xF021);
            const Str ShowInterface = EncodeUtf8CodePointV2(0xF06E);
            const Str HideInterface = EncodeUtf8CodePointV2(0xF070);
            const Str Save = EncodeUtf8CodePointV2(0xF0C7);
            const Str ZoomIn = EncodeUtf8CodePointV2(0xF00E);
            const Str ZoomOut = EncodeUtf8CodePointV2(0xF010);
            const Str Aspect11 = "1:1";
        }

        auto ComputeFloatingStripSizeV2(const size_t buttonCount,
                                        const float buttonSize,
                                        const float spacing,
                                        const float windowPadding,
                                        const bool vertical) -> ImVec2 {
            if (buttonCount == 0) return ImVec2{2.0f * windowPadding, 2.0f * windowPadding};

            const float stackedButtons = static_cast<float>(buttonCount) * buttonSize;
            const float stackedSpacing = static_cast<float>(buttonCount - 1) * spacing;
            const float stackedSize = stackedButtons + stackedSpacing;

            if (vertical) {
                return ImVec2{(2.0f * windowPadding) + buttonSize, (2.0f * windowPadding) + stackedSize};
            }

            return ImVec2{(2.0f * windowPadding) + stackedSize, (2.0f * windowPadding) + buttonSize};
        }

        auto BuildOverlayChromeLayoutV2(const Slab::Graphics::RectI &viewport,
                                        const bool bShowInterface) -> FOverlayChromeLayoutV2 {
            FOverlayChromeLayoutV2 layout;
            layout.ToolbarButtonSize = CBaseToolbarButtonSizeV2 * CToolbarButtonScaleV2;
            layout.RightButtonSize = CBaseRightButtonSizeV2 * CRightButtonScaleV2;
            layout.ToolbarSize = ComputeFloatingStripSizeV2(
                CToolbarButtonCountV2,
                layout.ToolbarButtonSize,
                COverlayStripSpacingV2,
                COverlayStripPaddingV2,
                false);
            layout.RightSize = ComputeFloatingStripSizeV2(
                CRightButtonCountV2,
                layout.RightButtonSize,
                COverlayStripSpacingV2,
                COverlayStripPaddingV2,
                true);

            const auto detailWidth = std::clamp(
                static_cast<float>(viewport.GetWidth()) * 0.30f,
                CDetailWidthMinV2,
                CDetailWidthMaxV2);
            const float panelDefaultX = static_cast<float>(viewport.xMax) - detailWidth - COverlayMarginV2;

            layout.ToolbarPos = {
                static_cast<float>(viewport.xMin) + COverlayMarginV2,
                static_cast<float>(viewport.yMin) + COverlayMarginV2
            };

            const float rightXMin = static_cast<float>(viewport.xMin) + COverlayMarginV2;
            const float rightXMax = std::max(
                rightXMin,
                static_cast<float>(viewport.xMax) - COverlayMarginV2 - layout.RightSize.x);
            float rightX = static_cast<float>(viewport.xMax) - COverlayMarginV2 - layout.RightSize.x;
            rightX = std::clamp(rightX, rightXMin, rightXMax);

            const float rightYMin = static_cast<float>(viewport.yMin) + COverlayMarginV2;
            const float rightYMax = static_cast<float>(viewport.yMax) - COverlayMarginV2 - layout.RightSize.y;
            const float rightYCentered =
                static_cast<float>(viewport.yMin) + 0.5f * (static_cast<float>(viewport.GetHeight()) - layout.RightSize.y);
            const float rightYTopAligned = rightYMin + std::max(0.0f, CRightStripTopOffsetV2);
            const float rightY = CRightStripAlignTopV2 ? rightYTopAligned : rightYCentered;
            layout.RightPos = {
                rightX,
                std::clamp(rightY, rightYMin, std::max(rightYMin, rightYMax))
            };

            if (!bShowInterface) return layout;

            const auto detailHeight = std::max(
                220.0f,
                static_cast<float>(viewport.GetHeight()) - 2.0f * COverlayMarginV2);
            float detailX = panelDefaultX;
            if (layout.RightPos.x + layout.RightSize.x + COverlayMarginV2 > panelDefaultX) {
                detailX = layout.RightPos.x - COverlayMarginV2 - detailWidth;
            }
            detailX = std::max(static_cast<float>(viewport.xMin) + COverlayMarginV2, detailX);

            layout.bHasDetailPanel = true;
            layout.DetailPos = {
                detailX,
                static_cast<float>(viewport.yMin) + COverlayMarginV2 + std::max(0.0f, CRightStripTopOffsetV2)
            };
            layout.DetailSize = {detailWidth, detailHeight};
            return layout;
        }

        auto LerpColorV2(const ImVec4 &from, const ImVec4 &to, const float t) -> ImVec4 {
            const auto clamped = std::clamp(t, 0.0f, 1.0f);
            return ImVec4{
                from.x + ((to.x - from.x) * clamped),
                from.y + ((to.y - from.y) * clamped),
                from.z + ((to.z - from.z) * clamped),
                from.w + ((to.w - from.w) * clamped)
            };
        }

        auto ColorWithAlphaV2(const ImVec4 &color, const float alpha) -> ImVec4 {
            return ImVec4{color.x, color.y, color.z, alpha};
        }

        auto RelativeLuminanceV2(const ImVec4 &color) -> float {
            return
                (0.2126f * color.x) +
                (0.7152f * color.y) +
                (0.0722f * color.z);
        }

        auto BuildFloatingStripPaletteV2(const bool transparentBackground) -> FFloatingStripPaletteV2 {
            const auto styleWindowBg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
            const auto styleButton = ImGui::GetStyleColorVec4(ImGuiCol_Button);
            const auto styleButtonHovered = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            const auto styleButtonActive = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
            const auto styleText = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            const auto styleTextDisabled = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
            const auto stylePopupBg = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);

            FFloatingStripPaletteV2 palette{
                styleButton,
                styleButtonHovered,
                styleButtonActive,
                styleButtonActive,
                styleButtonHovered,
                styleText,
                styleTextDisabled,
                stylePopupBg
            };

            if (!transparentBackground) return palette;

            const bool lightTheme = RelativeLuminanceV2(styleWindowBg) > 0.52f;
            const ImVec4 white{1.0f, 1.0f, 1.0f, 1.0f};
            const ImVec4 nearBlack{0.08f, 0.09f, 0.11f, 1.0f};
            const ImVec4 warmAccent{0.60f, 0.50f, 0.33f, 1.0f};

            if (lightTheme) {
                palette.Button = ColorWithAlphaV2(LerpColorV2(styleButton, white, 0.34f), 0.92f);
                palette.ButtonHovered = ColorWithAlphaV2(LerpColorV2(styleButtonHovered, white, 0.23f), 0.95f);
                palette.ButtonActive = ColorWithAlphaV2(LerpColorV2(styleButtonActive, white, 0.14f), 0.98f);
                palette.ActiveButton = ColorWithAlphaV2(
                    LerpColorV2(LerpColorV2(styleButtonActive, styleButtonHovered, 0.35f), warmAccent, 0.28f),
                    0.98f);
                palette.ActiveButtonHovered = ColorWithAlphaV2(LerpColorV2(palette.ActiveButton, white, 0.10f), 1.0f);
                palette.Text = ImVec4{0.13f, 0.14f, 0.17f, 0.99f};
                palette.TextDisabled = ImVec4{0.34f, 0.37f, 0.41f, 0.99f};
                palette.PopupBg = ColorWithAlphaV2(LerpColorV2(stylePopupBg, white, 0.12f), 0.97f);
            } else {
                palette.Button = ColorWithAlphaV2(LerpColorV2(styleButton, styleWindowBg, 0.10f), 0.92f);
                palette.ButtonHovered = ColorWithAlphaV2(LerpColorV2(styleButtonHovered, white, 0.10f), 0.95f);
                palette.ButtonActive = ColorWithAlphaV2(LerpColorV2(styleButtonActive, white, 0.08f), 0.98f);
                palette.ActiveButton = ColorWithAlphaV2(
                    LerpColorV2(LerpColorV2(styleButtonActive, styleButtonHovered, 0.28f), warmAccent, 0.20f),
                    0.98f);
                palette.ActiveButtonHovered = ColorWithAlphaV2(LerpColorV2(palette.ActiveButton, white, 0.18f), 1.0f);
                palette.Text = ImVec4{0.92f, 0.95f, 0.98f, 0.99f};
                palette.TextDisabled = ImVec4{0.68f, 0.73f, 0.79f, 0.99f};
                palette.PopupBg = ColorWithAlphaV2(LerpColorV2(stylePopupBg, nearBlack, 0.12f), 0.97f);
            }

            return palette;
        }

        auto DrawFloatingIconStripV2(const Str &windowLabel,
                                     const ImVec2 position,
                                     const bool vertical,
                                     const float buttonSize,
                                     const float spacing,
                                     const float windowPadding,
                                     const float backgroundAlpha,
                                     const bool transparentBackground,
                                     const Vector<FOverlayIconButtonV2> &buttons,
                                     const std::function<void()> &drawExtraContents = {}) -> void {
            if (buttons.empty()) return;

            ImGui::SetNextWindowPos(position, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(transparentBackground ? 0.0f : backgroundAlpha);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{windowPadding, windowPadding});
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{spacing, spacing});
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

            const auto palette = BuildFloatingStripPaletteV2(transparentBackground);
            int pushedColors = 0;
            if (transparentBackground) {
                ImGui::PushStyleColor(ImGuiCol_Button, palette.Button);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, palette.ButtonHovered);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, palette.ButtonActive);
                ImGui::PushStyleColor(ImGuiCol_Text, palette.Text);
                ImGui::PushStyleColor(ImGuiCol_TextDisabled, palette.TextDisabled);
                ImGui::PushStyleColor(ImGuiCol_PopupBg, palette.PopupBg);
                pushedColors = 6;
            }

            constexpr auto BaseFlags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_AlwaysAutoResize;

            auto windowFlags = BaseFlags;
            if (transparentBackground) {
                windowFlags |= ImGuiWindowFlags_NoBackground;
            }

            if (ImGui::Begin(windowLabel.c_str(), nullptr, windowFlags)) {
                for (size_t buttonIndex = 0; buttonIndex < buttons.size(); ++buttonIndex) {
                    const auto &button = buttons[buttonIndex];
                    const bool isActive = button.IsActive && button.IsActive();

                    if (isActive) {
                        const ImVec4 activeColor = transparentBackground
                            ? palette.ActiveButton
                            : ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
                        const ImVec4 hoveredColor = transparentBackground
                            ? palette.ActiveButtonHovered
                            : ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
                        ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, hoveredColor);
                    }

                    ImGui::PushID(button.Id.c_str());
                    const auto icon = button.GetIconLabel ? button.GetIconLabel() : Str("?");
                    if (ImGui::Button(icon.empty() ? "?" : icon.c_str(), ImVec2{buttonSize, buttonSize})) {
                        if (button.OnPressed) button.OnPressed();
                    }
                    ImGui::PopID();

                    if (isActive) {
                        ImGui::PopStyleColor(3);
                    }

                    if (!button.Tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(button.Tooltip.c_str());
                        ImGui::EndTooltip();
                    }

                    if (!vertical && (buttonIndex + 1) < buttons.size()) {
                        ImGui::SameLine();
                    }
                }

                if (drawExtraContents) {
                    drawExtraContents();
                }
            }

            ImGui::End();
            if (pushedColors > 0) {
                ImGui::PopStyleColor(pushedColors);
            }
            ImGui::PopStyleVar(5);
        }

    } // namespace

    auto FPlot2DWindowHostV2::BuildPointerEvent(const FPlot2DWindowV2 &window,
                                                const EPlotPointerEventKindV2 kind,
                                                const Slab::Graphics::FPoint2D &viewportPosition,
                                                const Slab::Graphics::EMouseButton button,
                                                const Slab::Graphics::EKeyState buttonState,
                                                const Slab::Graphics::EModKeys modKeys,
                                                const double wheelDx,
                                                const double wheelDy) const -> FPlotPointerEventV2 {
        return {
            .Kind = kind,
            .PlotPosition = window.ViewportToPlotCoord(viewportPosition),
            .ViewportPosition = viewportPosition,
            .Button = button,
            .ButtonState = buttonState,
            .ModKeys = modKeys,
            .WheelDx = wheelDx,
            .WheelDy = wheelDy
        };
    }

    auto FPlot2DWindowHostV2::FindWindow() const -> FPlot2DWindowV2 * {
        const auto windows = FPlot2DWindowV2::GetLiveWindows();
        const auto it = std::find_if(windows.begin(), windows.end(), [&](const auto *window) {
            return window != nullptr && window->GetWindowId() == PlotWindowId;
        });
        if (it == windows.end()) return nullptr;
        return *it;
    }

    auto FPlot2DWindowHostV2::IsRegionAnimating() const -> bool {
        return Slab::Graphics::FAnimator::Contains(AnimatedXMin) ||
               Slab::Graphics::FAnimator::Contains(AnimatedXMax) ||
               Slab::Graphics::FAnimator::Contains(AnimatedYMin) ||
               Slab::Graphics::FAnimator::Contains(AnimatedYMax);
    }

    auto FPlot2DWindowHostV2::SyncAnimationStateFromWindow(const FPlot2DWindowV2 &window) -> void {
        const auto region = window.GetRegion();
        if (!bAnimationStateInitialized || !IsRegionAnimating()) {
            AnimatedXMin = region.xMin;
            AnimatedXMax = region.xMax;
            AnimatedYMin = region.yMin;
            AnimatedYMax = region.yMax;
            TargetRegion = region;
            bAnimationStateInitialized = true;
        }
    }

    auto FPlot2DWindowHostV2::BuildAnimatedRegion() const -> Slab::Graphics::RectR {
        return {
            static_cast<Slab::DevFloat>(AnimatedXMin),
            static_cast<Slab::DevFloat>(AnimatedXMax),
            static_cast<Slab::DevFloat>(AnimatedYMin),
            static_cast<Slab::DevFloat>(AnimatedYMax)
        };
    }

    auto FPlot2DWindowHostV2::SetRegionImmediate(FPlot2DWindowV2 &window, const Slab::Graphics::RectR &region) -> void {
        AnimatedXMin = region.xMin;
        AnimatedXMax = region.xMax;
        AnimatedYMin = region.yMin;
        AnimatedYMax = region.yMax;
        TargetRegion = region;
        bAnimationStateInitialized = true;
        window.SetRegion(region);
    }

    auto FPlot2DWindowHostV2::AnimateRegionTo(const Slab::Graphics::RectR &region) -> void {
        TargetRegion = region;
        Slab::Graphics::FAnimator::Set(AnimatedXMin, region.xMin, AnimationTimeSeconds);
        Slab::Graphics::FAnimator::Set(AnimatedXMax, region.xMax, AnimationTimeSeconds);
        Slab::Graphics::FAnimator::Set(AnimatedYMin, region.yMin, AnimationTimeSeconds);
        Slab::Graphics::FAnimator::Set(AnimatedYMax, region.yMax, AnimationTimeSeconds);
        bAnimationStateInitialized = true;
    }

    auto FPlot2DWindowHostV2::EnforceUnitAspectRatio(Slab::Graphics::RectR &region) const -> void {
        if (!bLockUnitAspectRatio) return;

        const auto viewport = GetViewport();
        const auto viewportWidth = static_cast<Slab::DevFloat>(std::max(1, viewport.GetWidth()));
        const auto viewportHeight = static_cast<Slab::DevFloat>(std::max(1, viewport.GetHeight()));
        const auto viewportAspect = viewportWidth / viewportHeight;
        if (viewportAspect <= 0.0) return;

        const auto yCenter = region.yCenter();
        const auto halfWidth = static_cast<Slab::DevFloat>(
            std::max(0.5 * region.GetWidth(), static_cast<Slab::DevFloat>(1e-8)));
        const auto halfHeight = static_cast<Slab::DevFloat>(
            std::max(halfWidth / viewportAspect, static_cast<Slab::DevFloat>(1e-8)));
        region.yMin = yCenter - halfHeight;
        region.yMax = yCenter + halfHeight;
    }

    auto FPlot2DWindowHostV2::TryBuildFittedRegion(FPlot2DWindowV2 &window,
                                                   Slab::Graphics::RectR &fittedRegionOut,
                                                   const Slab::DevFloat paddingFraction) const -> bool {
        const auto previousRegion = window.GetRegion();
        if (!window.FitRegionToArtists(paddingFraction)) {
            window.SetRegion(previousRegion);
            return false;
        }

        fittedRegionOut = window.GetRegion();
        window.SetRegion(previousRegion);
        EnforceUnitAspectRatio(fittedRegionOut);
        return true;
    }

    auto FPlot2DWindowHostV2::AnimateToFittedRegion(FPlot2DWindowV2 &window,
                                                    const Slab::DevFloat paddingFraction) -> bool {
        Slab::Graphics::RectR fittedRegion{};
        if (!TryBuildFittedRegion(window, fittedRegion, paddingFraction)) return false;

        window.SetAutoFitRanges(false);
        AnimateRegionTo(fittedRegion);
        return true;
    }

    auto FPlot2DWindowHostV2::ZoomAroundCenter(FPlot2DWindowV2 &window, const Slab::DevFloat scale) -> void {
        auto region = BuildAnimatedRegion();
        const auto xCenter = region.xCenter();
        const auto yCenter = region.yCenter();
        const auto halfWidth = static_cast<Slab::DevFloat>(
            std::max(0.5 * region.GetWidth() * scale, static_cast<Slab::DevFloat>(1e-8)));
        const auto halfHeight = static_cast<Slab::DevFloat>(
            std::max(0.5 * region.GetHeight() * scale, static_cast<Slab::DevFloat>(1e-8)));
        region = {
            xCenter - halfWidth,
            xCenter + halfWidth,
            yCenter - halfHeight,
            yCenter + halfHeight
        };
        EnforceUnitAspectRatio(region);
        window.SetAutoFitRanges(false);
        AnimateRegionTo(region);
    }

    auto FPlot2DWindowHostV2::ToggleUnitAspectRatioLock(FPlot2DWindowV2 &window) -> void {
        SyncAnimationStateFromWindow(window);

        bLockUnitAspectRatio = !bLockUnitAspectRatio;
        if (!bLockUnitAspectRatio) return;

        auto region = BuildAnimatedRegion();
        EnforceUnitAspectRatio(region);
        window.SetAutoFitRanges(false);
        AnimateRegionTo(region);
    }

    auto FPlot2DWindowHostV2::BuildHudLayout(const Slab::Graphics::RectI &viewport) const -> FPlotHudLayoutV2 {
        const auto chrome = BuildOverlayChromeLayoutV2(viewport, bShowInterface);
        const auto left = static_cast<Slab::DevFloat>(viewport.xMin) + COverlayMarginV2;
        const auto bottom = static_cast<Slab::DevFloat>(viewport.yMin) + COverlayMarginV2;
        const auto top = std::max(bottom, static_cast<Slab::DevFloat>(viewport.yMax) - COverlayMarginV2);

        auto right = std::max(
            left,
            static_cast<Slab::DevFloat>(chrome.RightPos.x) - COverlayMarginV2 - CHudChromeGapV2);
        if (chrome.bHasDetailPanel) {
            right = std::max(
                left,
                std::min(
                    right,
                    static_cast<Slab::DevFloat>(chrome.DetailPos.x) - COverlayMarginV2 - CHudChromeGapV2));
        }

        auto topLeftX = static_cast<Slab::DevFloat>(
            chrome.ToolbarPos.x + chrome.ToolbarSize.x + COverlayMarginV2 + CHudChromeGapV2);
        topLeftX = std::clamp(topLeftX, left, right);

        return {
            .SafeRect = {left, right, bottom, top},
            .TopLeft = {topLeftX, top},
            .TopRight = {right, top},
            .BottomLeft = {left, bottom},
            .BottomRight = {right, bottom}
        };
    }

    auto FPlot2DWindowHostV2::SaveSnapshot() const -> void {
        const auto width = std::max(1, GetWidth());
        const auto height = std::max(1, GetHeight());
        const auto fileName = PlotWindowId + ".png";
        (void) Slab::Graphics::OpenGL::OutputToPNG(const_cast<FPlot2DWindowHostV2 *>(this), fileName, width, height);
    }

    auto FPlot2DWindowHostV2::IsOverlayEditableParameter(
        const FPlotReflectionParameterBindingV2 &parameter) -> bool {
        using namespace Slab::Core::Reflection::V2;

        if (parameter.Schema.Exposure != EParameterExposure::WritableExposed) return false;
        if (parameter.Schema.Mutability != EParameterMutability::RuntimeMutable) return false;
        if (parameter.Schema.ParameterId == "visible" ||
            parameter.Schema.ParameterId == "z_order" ||
            parameter.Schema.ParameterId == "label") {
            return false;
        }

        return parameter.Schema.TypeId == CTypeIdScalarBool ||
               parameter.Schema.TypeId == CTypeIdScalarInt32 ||
               parameter.Schema.TypeId == CTypeIdScalarFloat64;
    }

    auto FPlot2DWindowHostV2::DrawReflectionParameterControl(FPlotReflectionParameterBindingV2 &parameter) -> void {
        using namespace Slab::Core::Reflection::V2;

        if (!IsOverlayEditableParameter(parameter) || !parameter.ReadCurrent || !parameter.WriteLiveValue) return;

        const auto currentValue = parameter.ReadCurrent();
        ImGui::PushID(parameter.Schema.ParameterId.c_str());

        if (parameter.Schema.TypeId == CTypeIdScalarBool) {
            bool value = false;
            (void) ParseBoolValue(currentValue.Encoded, value);
            if (ImGui::Checkbox(parameter.Schema.DisplayName.c_str(), &value)) {
                (void) parameter.WriteLiveValue(MakeBoolValue(value));
            }
        } else if (parameter.Schema.TypeId == CTypeIdScalarInt32) {
            int value = 0;
            try {
                value = std::stoi(currentValue.Encoded);
            } catch (...) {
                value = 0;
            }

            const auto minValue = parameter.Schema.Minimum.has_value()
                ? static_cast<int>(*parameter.Schema.Minimum)
                : std::numeric_limits<int>::min();
            const auto maxValue = parameter.Schema.Maximum.has_value()
                ? static_cast<int>(*parameter.Schema.Maximum)
                : std::numeric_limits<int>::max();
            const auto speed = parameter.Schema.Step.value_or(1.0);

            bool changed = false;
            if (parameter.Schema.Minimum.has_value() || parameter.Schema.Maximum.has_value()) {
                changed = ImGui::SliderInt(parameter.Schema.DisplayName.c_str(), &value, minValue, maxValue);
            } else {
                changed = ImGui::DragInt(
                    parameter.Schema.DisplayName.c_str(),
                    &value,
                    static_cast<float>(speed),
                    minValue,
                    maxValue);
            }

            if (changed) {
                (void) parameter.WriteLiveValue(MakeIntValue(value));
            }
        } else if (parameter.Schema.TypeId == CTypeIdScalarFloat64) {
            float value = 0.0f;
            try {
                value = static_cast<float>(std::stod(currentValue.Encoded));
            } catch (...) {
                value = 0.0f;
            }

            const auto minValue = parameter.Schema.Minimum.value_or(-1.0e12);
            const auto maxValue = parameter.Schema.Maximum.value_or(1.0e12);
            const auto speed = parameter.Schema.Step.value_or(0.01);

            bool changed = false;
            if (parameter.Schema.Minimum.has_value() || parameter.Schema.Maximum.has_value()) {
                changed = ImGui::SliderFloat(
                    parameter.Schema.DisplayName.c_str(),
                    &value,
                    static_cast<float>(minValue),
                    static_cast<float>(maxValue));
            } else {
                changed = ImGui::DragFloat(
                    parameter.Schema.DisplayName.c_str(),
                    &value,
                    static_cast<float>(speed));
            }

            if (changed) {
                (void) parameter.WriteLiveValue(MakeFloatValue(static_cast<Slab::DevFloat>(value)));
            }
        }

        if (!parameter.Schema.Description.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 36.0f);
            ImGui::TextUnformatted(parameter.Schema.Description.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        ImGui::PopID();
    }

    auto FPlot2DWindowHostV2::DrawOverlay(FPlot2DWindowV2 &window) -> void {
        const auto viewport = GetViewport();
        if (viewport.GetWidth() <= 0 || viewport.GetHeight() <= 0) return;
        const auto chrome = BuildOverlayChromeLayoutV2(viewport, bShowInterface);

        const Vector<FOverlayIconButtonV2> toolbarButtons = {
            {
                "save",
                []() { return FontAwesomeSolidIconsV2::Save; },
                "Save plot as PNG",
                [this]() { SaveSnapshot(); },
                {}
            }
        };

        const Vector<FOverlayIconButtonV2> rightButtons = {
            {
                "zoom-in",
                []() { return FontAwesomeSolidIconsV2::ZoomIn; },
                "Zoom in",
                [this, &window]() { ZoomAroundCenter(window, 0.85); },
                {}
            },
            {
                "zoom-out",
                []() { return FontAwesomeSolidIconsV2::ZoomOut; },
                "Zoom out",
                [this, &window]() { ZoomAroundCenter(window, 1.15); },
                {}
            },
            {
                "aspect-11",
                []() { return FontAwesomeSolidIconsV2::Aspect11; },
                "Lock 1:1 unit aspect ratio",
                [this, &window]() { ToggleUnitAspectRatioLock(window); },
                [this]() { return bLockUnitAspectRatio; }
            },
            {
                "fit-right",
                []() { return FontAwesomeSolidIconsV2::Fit; },
                "Fit graph to visible artists",
                [this, &window]() { (void) AnimateToFittedRegion(window); },
                {}
            },
            {
                "auto-right",
                []() { return FontAwesomeSolidIconsV2::AutoAdjust; },
                "Toggle automatic fit",
                [&window]() { window.SetAutoFitRanges(!window.GetAutoFitRanges()); },
                [&window]() { return window.GetAutoFitRanges(); }
            },
            {
                "ui-right",
                [this]() { return bShowInterface ? FontAwesomeSolidIconsV2::HideInterface : FontAwesomeSolidIconsV2::ShowInterface; },
                "Show or hide detail panel",
                [this]() { bShowInterface = !bShowInterface; },
                [this]() { return bShowInterface; }
            }
        };

        const auto popupName = MakeOverlayId("PlotV2Context");
        DrawFloatingIconStripV2(
            MakeOverlayId("PlotV2Toolbar"),
            chrome.ToolbarPos,
            false,
            chrome.ToolbarButtonSize,
            COverlayStripSpacingV2,
            COverlayStripPaddingV2,
            CToolbarBackgroundAlphaV2,
            CTransparentBackgroundV2,
            toolbarButtons,
            [this, &window, &popupName]() {
                if (bPendingContextPopup) {
                    ImGui::OpenPopup(popupName.c_str());
                    bPendingContextPopup = false;
                }

                if (ImGui::BeginPopup(popupName.c_str())) {
                    const bool autoFitPopup = window.GetAutoFitRanges();
                    if (ImGui::MenuItem("Auto fit", nullptr, autoFitPopup)) {
                        window.SetAutoFitRanges(!autoFitPopup);
                    }
                    if (ImGui::MenuItem("Show detail", nullptr, bShowInterface)) {
                        bShowInterface = !bShowInterface;
                    }
                    if (ImGui::MenuItem("Fit now")) {
                        (void) AnimateToFittedRegion(window);
                    }
                    if (ImGui::MenuItem("Lock 1:1", nullptr, bLockUnitAspectRatio)) {
                        ToggleUnitAspectRatioLock(window);
                    }
                    if (ImGui::MenuItem("Save graph")) {
                        SaveSnapshot();
                    }
                    ImGui::EndPopup();
                }
            });

        DrawFloatingIconStripV2(
            MakeOverlayId("PlotV2RightControls"),
            chrome.RightPos,
            true,
            chrome.RightButtonSize,
            COverlayStripSpacingV2,
            COverlayStripPaddingV2,
            CRightBackgroundAlphaV2,
            CTransparentBackgroundV2,
            rightButtons);

        if (!bShowInterface) return;

        ImGui::SetNextWindowPos(chrome.DetailPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(chrome.DetailSize, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.80f);
        constexpr auto DetailFlags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin(MakeOverlayId("PlotV2Detail").c_str(), nullptr, DetailFlags)) {
            if (ImGui::CollapsingHeader("View", ImGuiTreeNodeFlags_DefaultOpen)) {
                const auto region = BuildAnimatedRegion();
                ImGui::Text("Auto fit: %s", window.GetAutoFitRanges() ? "On" : "Off");
                ImGui::Text("Detail panel: %s", bShowInterface ? "Visible" : "Hidden");
                ImGui::Text("1:1 lock: %s", bLockUnitAspectRatio ? "On" : "Off");
                ImGui::Text("x:[%.4g, %.4g]  y:[%.4g, %.4g]",
                    region.xMin,
                    region.xMax,
                    region.yMin,
                    region.yMax);
                ImGui::Text("w=%.4g  h=%.4g", region.GetWidth(), region.GetHeight());
                if (ImGui::Button("Fit To Artists")) {
                    (void) AnimateToFittedRegion(window);
                }
                ImGui::SameLine();
                if (ImGui::Button("Save PNG")) {
                    SaveSnapshot();
                }
            }

            if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto orderedArtists = window.GetArtistsInDrawOrder();
                for (const auto &slot : orderedArtists) {
                    if (slot.Artist == nullptr) continue;

                    int zOrder = slot.ZOrder;
                    (void) window.TryGetArtistZOrder(slot.Artist, zOrder);
                    bool visible = slot.Artist->IsVisible();

                    ImGui::PushID(slot.Artist.get());
                    if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
                        (void) window.SetArtistZOrder(slot.Artist, zOrder + 1);
                    }
                    ImGui::SameLine();
                    if (ImGui::ArrowButton("##down", ImGuiDir_Down)) {
                        (void) window.SetArtistZOrder(slot.Artist, zOrder - 1);
                    }
                    ImGui::SameLine();
                    if (ImGui::Checkbox("##visible", &visible)) {
                        slot.Artist->SetVisible(visible);
                    }
                    ImGui::SameLine();
                    ImGui::TextUnformatted(slot.Artist->GetLabel().c_str());
                    ImGui::SameLine();
                    ImGui::TextDisabled("(z=%d)", zOrder);
                    ImGui::PopID();
                }
            }

            if (ImGui::CollapsingHeader("Artist Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
                const auto availRegion = ImGui::GetContentRegionAvail();
                ImGui::BeginChild(
                    MakeOverlayId("PlotV2ArtistControls").c_str(),
                    ImVec2(availRegion.x, 0.0f),
                    ImGuiChildFlags_Border);

                for (const auto &slot : window.GetArtistsInDrawOrder()) {
                    if (slot.Artist == nullptr || !slot.Artist->IsVisible()) continue;

                    auto descriptor = slot.Artist->DescribeReflection();
                    const bool hasEditableControls = std::any_of(
                        descriptor.Parameters.begin(),
                        descriptor.Parameters.end(),
                        [](const auto &parameter) { return IsOverlayEditableParameter(parameter); });
                    if (!hasEditableControls) continue;

                    if (ImGui::CollapsingHeader(
                            (slot.Artist->GetLabel() + "##artist_controls").c_str(),
                            ImGuiTreeNodeFlags_DefaultOpen)) {
                        for (auto &parameter : descriptor.Parameters) {
                            DrawReflectionParameterControl(parameter);
                        }
                    }
                }

                ImGui::EndChild();
            }
        }
        ImGui::End();
    }

    FPlot2DWindowHostV2::FPlot2DWindowHostV2(Str plotWindowId,
                                             Str title,
                                             Slab::TPointer<IPlotRenderBackendV2> renderBackend)
    : FSlabWindow(Slab::Graphics::FSlabWindowConfig(std::move(title)))
    , PlotWindowId(std::move(plotWindowId))
    , RenderBackend(renderBackend != nullptr
        ? std::move(renderBackend)
        : Slab::New<FOpenGLRenderBackendV2>()) {
        Slab::Core::LoadModule("RealTimeAnimation");
    }

    auto FPlot2DWindowHostV2::RequestFitToArtists() -> void {
        bPendingFitToArtists = true;
    }

    auto FPlot2DWindowHostV2::NotifyMouseButton(const Slab::Graphics::EMouseButton button,
                                                const Slab::Graphics::EKeyState state,
                                                const Slab::Graphics::EModKeys keys) -> bool {
        if (FSlabWindow::NotifyMouseButton(button, state, keys)) return true;

        auto *window = FindWindow();
        if (window == nullptr) return false;

        SyncAnimationStateFromWindow(*window);
        const auto viewportPosition = GetMouseViewportCoord();
        const auto frame = window->BuildFrameContext(BuildHudLayout(window->GetViewport()));

        if (state == Slab::Graphics::Press) {
            if (button == Slab::Graphics::MouseButton_LEFT) {
                LeftMouseDownViewportPosition = viewportPosition;
                bLeftDragCommitted = false;

                const auto plotPosition = window->ViewportToPlotCoord(viewportPosition);
                bLeftPressStartedOnInteractiveTarget =
                    window->HitTestArtists(frame, plotPosition, viewportPosition).has_value();
                return window->DispatchPointerEvent(
                    frame,
                    BuildPointerEvent(*window, EPlotPointerEventKindV2::Button, viewportPosition, button, state, keys));
            }

            if (button == Slab::Graphics::MouseButton_RIGHT) {
                RightMouseDownViewportPosition = viewportPosition;
                bRightDragCommitted = false;
            }

            return false;
        }

        if (state == Slab::Graphics::Release) {
            if (button == Slab::Graphics::MouseButton_LEFT) {
                const bool bDispatchRelease = !bLeftDragCommitted;
                bLeftDragCommitted = false;
                bLeftPressStartedOnInteractiveTarget = false;
                if (!bDispatchRelease) return false;

                return window->DispatchPointerEvent(
                    frame,
                    BuildPointerEvent(*window, EPlotPointerEventKindV2::Button, viewportPosition, button, state, keys));
            }

            if (button == Slab::Graphics::MouseButton_RIGHT) {
                if (!bRightDragCommitted) {
                    bPendingContextPopup = true;
                    bRightDragCommitted = false;
                    return true;
                }
                bRightDragCommitted = false;
            }
        }

        return false;
    }

    auto FPlot2DWindowHostV2::NotifyKeyboard(const Slab::Graphics::EKeyMap key,
                                             const Slab::Graphics::EKeyState state,
                                             const Slab::Graphics::EModKeys modKeys) -> bool {
        if (FSlabWindow::NotifyKeyboard(key, state, modKeys)) return true;

        auto *window = FindWindow();
        if (window == nullptr) return false;

        SyncAnimationStateFromWindow(*window);
        const auto frame = window->BuildFrameContext(BuildHudLayout(window->GetViewport()));

        if (state == Slab::Graphics::Release &&
            key == Slab::Graphics::Key_TAB &&
            modKeys.Mod_Shift == Slab::Graphics::Press) {
            bShowInterface = !bShowInterface;
            return true;
        }

        if (state == Slab::Graphics::Press || state == Slab::Graphics::Repeat) {
            if (key == Slab::Graphics::Key_F || key == Slab::Graphics::Key_f) {
                return AnimateToFittedRegion(*window);
            }
        }

        const auto handled = window->DispatchKeyboardEvent(
            frame,
            FPlotKeyboardEventV2{
                .Key = key,
                .State = state,
                .ModKeys = modKeys
            });
        if (!handled) return false;

        if ((state == Slab::Graphics::Press || state == Slab::Graphics::Repeat) &&
            (key == Slab::Graphics::Key_LEFT_BRACKET ||
             key == Slab::Graphics::Key_RIGHT_BRACKET ||
             key == Slab::Graphics::Key_MINUS ||
             key == Slab::Graphics::Key_EQUAL ||
             key == Slab::Graphics::Key_PLUS ||
             key == Slab::Graphics::Key_KP_SUBTRACT ||
             key == Slab::Graphics::Key_KP_ADD)) {
            (void) AnimateToFittedRegion(*window);
        }

        return true;
    }

    auto FPlot2DWindowHostV2::NotifyMouseMotion(const int x, const int y, const int dx, const int dy) -> bool {
        if (FSlabWindow::NotifyMouseMotion(x, y, dx, dy)) return true;

        auto *window = FindWindow();
        if (window == nullptr) return false;

        SyncAnimationStateFromWindow(*window);

        const auto mouseState = GetMouseState();
        if (mouseState == nullptr) return false;

        const auto viewportPosition = GetMouseViewportCoord();
        const auto frame = window->BuildFrameContext(BuildHudLayout(window->GetViewport()));
        constexpr Slab::DevFloat DragThresholdSq = 6.0 * 6.0;

        auto region = BuildAnimatedRegion();
        bool bHandled = false;

        if (mouseState->IsLeftPressed()) {
            const auto dragDistanceSq = DistanceSquared(viewportPosition, LeftMouseDownViewportPosition);
            if (!bLeftPressStartedOnInteractiveTarget && dragDistanceSq >= DragThresholdSq) {
                bLeftDragCommitted = true;
            }

            if (bLeftDragCommitted) {
                const auto viewportWidth = static_cast<Slab::DevFloat>(std::max(1, GetWidth()));
                const auto viewportHeight = static_cast<Slab::DevFloat>(std::max(1, GetHeight()));

                const auto dxClamped = -static_cast<Slab::DevFloat>(mouseState->dx) / viewportWidth;
                const auto dyClamped = static_cast<Slab::DevFloat>(mouseState->dy) / viewportHeight;

                const auto dxGraph = region.GetWidth() * dxClamped;
                const auto dyGraph = region.GetHeight() * dyClamped;

                region = {
                    region.xMin + dxGraph,
                    region.xMax + dxGraph,
                    region.yMin + dyGraph,
                    region.yMax + dyGraph
                };

                window->SetAutoFitRanges(false);
                SetRegionImmediate(*window, region);
                bHandled = true;
            } else {
                bHandled = window->DispatchPointerEvent(
                    frame,
                    BuildPointerEvent(*window, EPlotPointerEventKindV2::Move, viewportPosition));
            }
        }

        if (mouseState->IsRightPressed()) {
            const auto dragDistanceSq = DistanceSquared(viewportPosition, RightMouseDownViewportPosition);
            if (dragDistanceSq >= DragThresholdSq) {
                bRightDragCommitted = true;
            }

            if (bRightDragCommitted) {
                if (bLockUnitAspectRatio) {
                    constexpr Slab::DevFloat factor = 0.01;
                    const auto d = static_cast<Slab::DevFloat>(1.0 - factor * static_cast<Slab::DevFloat>(dx - dy));
                    const auto x0 = region.xCenter();
                    const auto hw = static_cast<Slab::DevFloat>(0.5 * region.GetWidth() * d);
                    region = {x0 - hw, x0 + hw, region.yMin, region.yMax};
                    EnforceUnitAspectRatio(region);
                } else {
                    constexpr Slab::DevFloat factor = 0.01;
                    const auto dw = static_cast<Slab::DevFloat>(1.0 - factor * static_cast<Slab::DevFloat>(dx));
                    const auto dh = static_cast<Slab::DevFloat>(1.0 + factor * static_cast<Slab::DevFloat>(dy));

                    const auto x0 = region.xCenter();
                    const auto y0 = region.yCenter();
                    const auto hw = static_cast<Slab::DevFloat>(0.5 * region.GetWidth() * dw);
                    const auto hh = static_cast<Slab::DevFloat>(0.5 * region.GetHeight() * dh);
                    region = {x0 - hw, x0 + hw, y0 - hh, y0 + hh};
                }

                window->SetAutoFitRanges(false);
                SetRegionImmediate(*window, region);
                bHandled = true;
            }
        }

        if (!mouseState->IsLeftPressed() && !mouseState->IsRightPressed()) {
            bHandled = window->DispatchPointerEvent(
                frame,
                BuildPointerEvent(*window, EPlotPointerEventKindV2::Move, viewportPosition)) || bHandled;
        }

        return bHandled;
    }

    auto FPlot2DWindowHostV2::NotifyMouseWheel(const double dx, const double dy) -> bool {
        if (FSlabWindow::NotifyMouseWheel(dx, dy)) return true;
        if (std::abs(dy) < 1.0e-12) return false;

        auto *window = FindWindow();
        if (window == nullptr) return false;

        SyncAnimationStateFromWindow(*window);
        const auto viewportPosition = GetMouseViewportCoord();
        const auto frame = window->BuildFrameContext(BuildHudLayout(window->GetViewport()));
        if (window->DispatchPointerEvent(
                frame,
                BuildPointerEvent(*window,
                                  EPlotPointerEventKindV2::Wheel,
                                  viewportPosition,
                                  Slab::Graphics::MouseButton_LEFT,
                                  Slab::Graphics::Release,
                                  {},
                                  dx,
                                  dy))) {
            return true;
        }

        constexpr Slab::DevFloat factor = 1.2;
        const auto d = std::pow(factor, -dy);

        auto targetRegion = TargetRegion;
        if (!IsRegionAnimating()) {
            targetRegion = BuildAnimatedRegion();
        }

        const auto x0 = targetRegion.xCenter();
        const auto hw = static_cast<Slab::DevFloat>(0.5 * targetRegion.GetWidth() * d);
        targetRegion.xMin = x0 - hw;
        targetRegion.xMax = x0 + hw;

        const auto y0 = targetRegion.yCenter();
        const auto hh = static_cast<Slab::DevFloat>(0.5 * targetRegion.GetHeight() * d);
        targetRegion.yMin = y0 - hh;
        targetRegion.yMax = y0 + hh;

        EnforceUnitAspectRatio(targetRegion);
        window->SetAutoFitRanges(false);
        AnimateRegionTo(targetRegion);

        return true;
    }

    auto FPlot2DWindowHostV2::RegisterDeferredDrawCalls(const Slab::Graphics::FPlatformWindow &platformWindow) -> void {
        FSlabWindow::RegisterDeferredDrawCalls(platformWindow);

        auto *window = FindWindow();
        if (window == nullptr) return;
        DrawOverlay(*window);
    }

    auto FPlot2DWindowHostV2::ImmediateDraw(const Slab::Graphics::FPlatformWindow &platformWindow) -> void {
        FSlabWindow::ImmediateDraw(platformWindow);

        auto *window = FindWindow();
        if (window == nullptr) {
            Close();
            return;
        }

        const auto width = std::max(1, GetWidth());
        const auto height = std::max(1, GetHeight());
        window->SetViewport({0, width, 0, height});

        if (!window->GetTitle().empty()) {
            GetConfig().Title = window->GetTitle();
        }

        SyncAnimationStateFromWindow(*window);
        if (bPendingFitToArtists) {
            Slab::Graphics::RectR fittedRegion{};
            if (TryBuildFittedRegion(*window, fittedRegion)) {
                SetRegionImmediate(*window, fittedRegion);
            }
            bPendingFitToArtists = false;
        }

        const bool bAutoFitRanges = window->GetAutoFitRanges();
        if (bAutoFitRanges) {
            Slab::Graphics::RectR fittedRegion{};
            if (TryBuildFittedRegion(*window, fittedRegion)) {
                SetRegionImmediate(*window, fittedRegion);
            }
        }
        window->SetRegion(BuildAnimatedRegion());
        if (bAutoFitRanges) window->SetAutoFitRanges(false);
        if (RenderBackend == nullptr) return;
        const auto frame = window->BuildFrameContext(BuildHudLayout(window->GetViewport()));
        (void) window->Render(*RenderBackend, frame);
        if (bAutoFitRanges) window->SetAutoFitRanges(true);
    }

    auto FPlot2DWindowHostV2::GetPlotWindowId() const -> const Str & {
        return PlotWindowId;
    }

} // namespace Slab::Graphics::Plot2D::V2
