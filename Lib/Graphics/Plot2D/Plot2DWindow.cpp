//
// Created by joao on 29/05/23.
//

#include "Core/Tools/Log.h"

#include "Graphics/SlabGraphics.h"

#include "Plot2DWindow.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <ranges>

#include "PlotStyle.h"

#include <utility>

#include "3rdParty/ImGui.h"

#include "Utils/Printing.h"

#include "../../Core/Controller/InterfaceManager.h"

#include "PlotThemeManager.h"
#include "Artists/AxisArtist.h"

#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Core/SlabCore.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "StudioSlab.h"
#include "Graphics/OpenGL/Shader.h"

namespace Slab::Graphics {

    using Core::FLog;

    namespace {

        auto NormalizeToken(Str raw) -> Str
        {
            for (char &ch : raw) {
                if (std::isalnum(static_cast<unsigned char>(ch))) {
                    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                } else {
                    ch = '_';
                }
            }

            Str out;
            out.reserve(raw.size());

            bool lastUnderscore = false;
            for (const char ch : raw) {
                const bool isUnderscore = ch == '_';
                if (isUnderscore && lastUnderscore) continue;
                out.push_back(ch);
                lastUnderscore = isUnderscore;
            }

            while (!out.empty() && out.front() == '_') out.erase(out.begin());
            while (!out.empty() && out.back() == '_') out.pop_back();

            return out;
        }

        auto BuildDefaultArtistId(const FPlot2DWindow::ContentMap &content, const FArtist_ptr &pArtist) -> Str
        {
            auto base = NormalizeToken(pArtist->GetLabel());
            if (base.empty()) base = "artist";

            const auto exists = [&content](const Str &candidate) {
                return std::any_of(content.begin(), content.end(), [&](const auto &slot) {
                    const auto &existing = slot.second;
                    return existing != nullptr && existing->GetArtistId() == candidate;
                });
            };

            if (!exists(base)) return base;

            int suffix = 2;
            while (true) {
                const auto candidate = base + "_" + ToStr(suffix);
                if (!exists(candidate)) return candidate;
                ++suffix;
            }
        }

        struct FOverlayIconButton {
            Str Id;
            std::function<Str()> GetIconLabel;
            Str Tooltip;
            std::function<void()> OnPressed;
            std::function<bool()> IsActive;
        };

        auto EncodeUtf8CodePoint(const unsigned int codePoint) -> Str
        {
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

        namespace FontAwesomeSolidIcons {
            const Str Fit = EncodeUtf8CodePoint(0xF065);
            const Str AutoAdjust = EncodeUtf8CodePoint(0xF021);
            const Str ShowInterface = EncodeUtf8CodePoint(0xF06E);
            const Str HideInterface = EncodeUtf8CodePoint(0xF070);
            const Str Save = EncodeUtf8CodePoint(0xF0C7);
            const Str ZoomIn = EncodeUtf8CodePoint(0xF00E);
            const Str ZoomOut = EncodeUtf8CodePoint(0xF010);
            const Str Aspect11 = "1:1";
        }

        auto ComputeFloatingStripSize(
            const size_t buttonCount,
            const float buttonSize,
            const float spacing,
            const float windowPadding,
            const bool vertical) -> ImVec2
        {
            if (buttonCount == 0) return ImVec2{2.0f * windowPadding, 2.0f * windowPadding};

            const float stackedButtons = static_cast<float>(buttonCount) * buttonSize;
            const float stackedSpacing = static_cast<float>(buttonCount - 1) * spacing;
            const float stackedSize = stackedButtons + stackedSpacing;

            if (vertical) {
                return ImVec2{(2.0f * windowPadding) + buttonSize, (2.0f * windowPadding) + stackedSize};
            }

            return ImVec2{(2.0f * windowPadding) + stackedSize, (2.0f * windowPadding) + buttonSize};
        }

        struct FFloatingStripPalette {
            ImVec4 Button = {};
            ImVec4 ButtonHovered = {};
            ImVec4 ButtonActive = {};
            ImVec4 ActiveButton = {};
            ImVec4 ActiveButtonHovered = {};
            ImVec4 Text = {};
            ImVec4 TextDisabled = {};
            ImVec4 PopupBg = {};
        };

        auto LerpColor(const ImVec4& from, const ImVec4& to, const float t) -> ImVec4
        {
            const auto clamped = std::clamp(t, 0.0f, 1.0f);
            return ImVec4{
                from.x + ((to.x - from.x) * clamped),
                from.y + ((to.y - from.y) * clamped),
                from.z + ((to.z - from.z) * clamped),
                from.w + ((to.w - from.w) * clamped)};
        }

        auto ColorWithAlpha(const ImVec4& color, const float alpha) -> ImVec4
        {
            return ImVec4{color.x, color.y, color.z, alpha};
        }

        auto RelativeLuminance(const ImVec4& color) -> float
        {
            return
                (0.2126f * color.x) +
                (0.7152f * color.y) +
                (0.0722f * color.z);
        }

        auto BuildFloatingStripPalette(const bool transparentBackground) -> FFloatingStripPalette
        {
            const auto styleWindowBg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
            const auto styleButton = ImGui::GetStyleColorVec4(ImGuiCol_Button);
            const auto styleButtonHovered = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            const auto styleButtonActive = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
            const auto styleText = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            const auto styleTextDisabled = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
            const auto stylePopupBg = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);

            FFloatingStripPalette palette{
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

            const bool lightTheme = RelativeLuminance(styleWindowBg) > 0.52f;
            const ImVec4 white{1.0f, 1.0f, 1.0f, 1.0f};
            const ImVec4 nearBlack{0.08f, 0.09f, 0.11f, 1.0f};
            const ImVec4 warmAccent{0.60f, 0.50f, 0.33f, 1.0f};

            if (lightTheme) {
                palette.Button = ColorWithAlpha(LerpColor(styleButton, white, 0.34f), 0.92f);
                palette.ButtonHovered = ColorWithAlpha(LerpColor(styleButtonHovered, white, 0.23f), 0.95f);
                palette.ButtonActive = ColorWithAlpha(LerpColor(styleButtonActive, white, 0.14f), 0.98f);
                palette.ActiveButton = ColorWithAlpha(
                    LerpColor(LerpColor(styleButtonActive, styleButtonHovered, 0.35f), warmAccent, 0.28f),
                    0.98f);
                palette.ActiveButtonHovered = ColorWithAlpha(LerpColor(palette.ActiveButton, white, 0.10f), 1.0f);
                palette.Text = ImVec4{0.13f, 0.14f, 0.17f, 0.99f};
                palette.TextDisabled = ImVec4{0.34f, 0.37f, 0.41f, 0.99f};
                palette.PopupBg = ColorWithAlpha(LerpColor(stylePopupBg, white, 0.12f), 0.97f);
            } else {
                palette.Button = ColorWithAlpha(LerpColor(styleButton, styleWindowBg, 0.10f), 0.92f);
                palette.ButtonHovered = ColorWithAlpha(LerpColor(styleButtonHovered, white, 0.10f), 0.95f);
                palette.ButtonActive = ColorWithAlpha(LerpColor(styleButtonActive, white, 0.08f), 0.98f);
                palette.ActiveButton = ColorWithAlpha(
                    LerpColor(LerpColor(styleButtonActive, styleButtonHovered, 0.28f), warmAccent, 0.20f),
                    0.98f);
                palette.ActiveButtonHovered = ColorWithAlpha(LerpColor(palette.ActiveButton, white, 0.18f), 1.0f);
                palette.Text = ImVec4{0.92f, 0.95f, 0.98f, 0.99f};
                palette.TextDisabled = ImVec4{0.68f, 0.73f, 0.79f, 0.99f};
                palette.PopupBg = ColorWithAlpha(LerpColor(stylePopupBg, nearBlack, 0.12f), 0.97f);
            }

            return palette;
        }

        auto DrawFloatingIconStrip(
            const Str& windowLabel,
            const ImVec2 position,
            const bool vertical,
            const float buttonSize,
            const float spacing,
            const float windowPadding,
            const float backgroundAlpha,
            const bool transparentBackground,
            const Vector<FOverlayIconButton>& buttons,
            const std::function<void()>& drawExtraContents = {}) -> void
        {
            if (buttons.empty()) return;

            ImGui::SetNextWindowPos(position, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(transparentBackground ? 0.0f : backgroundAlpha);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{windowPadding, windowPadding});
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{spacing, spacing});
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

            const auto palette = BuildFloatingStripPalette(transparentBackground);
            auto nPushedStripColors = 0;
            if (transparentBackground) {
                ImGui::PushStyleColor(ImGuiCol_Button, palette.Button);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, palette.ButtonHovered);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, palette.ButtonActive);
                ImGui::PushStyleColor(ImGuiCol_Text, palette.Text);
                ImGui::PushStyleColor(ImGuiCol_TextDisabled, palette.TextDisabled);
                ImGui::PushStyleColor(ImGuiCol_PopupBg, palette.PopupBg);
                nPushedStripColors = 6;
            }

            constexpr auto BaseToolbarFlags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_AlwaysAutoResize;

            auto toolbarFlags = BaseToolbarFlags;
            if (transparentBackground) {
                toolbarFlags |= ImGuiWindowFlags_NoBackground;
            }

            if (ImGui::Begin(windowLabel.c_str(), nullptr, toolbarFlags))
            {
                for (size_t buttonIndex = 0; buttonIndex < buttons.size(); ++buttonIndex)
                {
                    const auto& button = buttons[buttonIndex];
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
                    const Str icon = button.GetIconLabel ? button.GetIconLabel() : Str("?");
                    if (ImGui::Button(icon.empty() ? "?" : icon.c_str(), ImVec2{buttonSize, buttonSize}))
                    {
                        if (button.OnPressed) button.OnPressed();
                    }
                    ImGui::PopID();

                    if (isActive) {
                        ImGui::PopStyleColor(3);
                    }

                    if (!button.Tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    {
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
            if (nPushedStripColors > 0) {
                ImGui::PopStyleColor(nPushedStripColors);
            }
            ImGui::PopStyleVar(5);
        }

    } // namespace

    std::map<Str, FPlot2DWindow *> FPlot2DWindow::GraphMap = {};
    CountType FPlot2DWindow::WindowCount = 0;
    FPlot2DWindow::FOverlayControlsStyle FPlot2DWindow::OverlayControlsStyle = {};

    using Mappy = FPlot2DWindow::ContentMap;
    bool change_z_order(Mappy& mappy, Mappy::iterator &it, int z_order){
        if(z_order == it->first) return false;

        auto artist = it->second;
        it = mappy.erase(it);
        mappy.insert(std::make_pair(z_order, artist));

        return true;
    }
    bool z_order_up  (Mappy& mappy, Mappy::iterator it) { return change_z_order(mappy, it, it->first+1); }
    bool z_order_down(Mappy& mappy, Mappy::iterator it) { return change_z_order(mappy, it, it->first-1); }

    FPlot2DWindow::FPlot2DWindow(
        DevFloat xMin,
        DevFloat xMax,
        DevFloat yMin,
        DevFloat yMax,
        Str _title)
    : FSlabWindow(FSlabWindowConfig(_title))
    , Id(++WindowCount)
    , Region{{xMin, xMax, yMin, yMax}}
    , Title(std::move(_title))
    {
        // Instantiate our dedicated Plot themes manager
        FPlotThemeManager::GetInstance();

        AxisArtist.SetLabel("Axis");
        ArtistXHair.SetLabel("X-hair");
        LabelsArtist.SetLabel("Labels");

        if (Title.empty()) Title = Str("unnamed");
        CountType n = 1;
        {
            Str UniqueTitle = Title;
            while (GraphMap.contains(UniqueTitle))
                UniqueTitle = Title + "(" + ToStr(++n) + ")";
            Title = UniqueTitle;
        }
        GraphMap[Title] = this;

        Core::LoadModule("ImGui");
        Core::LoadModule("RealTimeAnimation");

        FLog::Note() << "Created PlottingWindow '" << Title << "'" << FLog::Flush;

        AddArtist(Naked(AxisArtist), 5);
        AddArtist(Naked(LabelsArtist), 5);
        AddArtist(Naked(ArtistXHair), 6);
        AddArtist(Naked(bgArtist), -10);

    }

    FPlot2DWindow::FPlot2DWindow(Str Title)
    : FPlot2DWindow(-1, 1, -1, 1, std::move(Title)) {    }

    FPlot2DWindow::~FPlot2DWindow() {
        const auto it = GraphMap.find(Title);
        if (it != GraphMap.end() && it->second == this) {
            GraphMap.erase(it);
        }
    }

    auto FPlot2DWindow::GetLiveWindows() -> Vector<FPlot2DWindow *> {
        Vector<FPlot2DWindow *> windows;
        windows.reserve(GraphMap.size());

        for (const auto &[windowTitle, window] : GraphMap) {
            (void) windowTitle;
            if (window == nullptr) continue;
            windows.push_back(window);
        }

        return windows;
    }

    auto FPlot2DWindow::FindLiveWindowByTitle(const Str &title) -> FPlot2DWindow * {
        const auto it = GraphMap.find(title);
        if (it == GraphMap.end()) return nullptr;
        return it->second;
    }

    void FPlot2DWindow::AddArtist(const FArtist_ptr &pArtist, zOrder_t zOrder) {
        if (pArtist == nullptr) {
            FLog::Error() << __PRETTY_FUNCTION__ << " trying to add "
                         << FLog::FGBlue << "nullptr" << FLog::ResetFormatting << " artist.";

            return;
        }

        if (pArtist->GetArtistId().empty()) {
            pArtist->SetArtistId(BuildDefaultArtistId(Content, pArtist));
        }

        Content.emplace(zOrder, pArtist);

        FLog::Note()
        << "PlottingWindow '" << this->Title << "' added artist '"
        << pArtist->GetLabel() << "'." << FLog::Flush;
    }

    bool FPlot2DWindow::RemoveArtist(const Graphics::FArtist_ptr &pArtist) {
        auto haveErased = false;

        for (auto item = Content.begin(); item != Content.end();) {
            if (item->second == pArtist) {
                item = Content.erase(item);
                haveErased = true;
            } else ++item;
        }

        return haveErased;
    }

    void FPlot2DWindow::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        OpenGL::CheckGLErrors(Str(__PRETTY_FUNCTION__) + "; '" + Title + "'");

        FSlabWindow::ImmediateDraw(PlatformWindow);

        if (AutoReviewGraphRanges) ReviewGraphRanges();

        ArtistsDraw();
    }

    void FPlot2DWindow::RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow)
    {
        FSlabWindow::RegisterDeferredDrawCalls(PlatformWindow);

        RegisterGUIDraws();
    }

    void FPlot2DWindow::SetNoGUI() {
        b_DrawGUI = false;
    }

    void FPlot2DWindow::SetGlobalOverlayControlsStyle(const FOverlayControlsStyle& style)
    {
        OverlayControlsStyle = style;
    }

    auto FPlot2DWindow::GetGlobalOverlayControlsStyle() -> FOverlayControlsStyle
    {
        return OverlayControlsStyle;
    }

    void FPlot2DWindow::RegisterGUIDraws() {
        auto DrawCall = [this]
        {
            const auto Viewport = GetViewport();
            if (Viewport.GetWidth() <= 0 || Viewport.GetHeight() <= 0) return;

            auto SaveGraphToDisk = [this]() {
                auto w = Printing::GetTotalHorizontalDots(.5);
                auto h = w * .5;
                auto fileName = Title + " " +
                                Core::FInterfaceManager::GetInstance().RenderParametersToString({"N", "L"}) +
                                ".png";

                OpenGL::OutputToPNG(this, fileName, w, (int) h);
            };

            auto AnimateToRegion = [this](const RectR& rect) {
                Region.animate_xMin(rect.xMin);
                Region.animate_xMax(rect.xMax);
                Region.animate_yMin(rect.yMin);
                Region.animate_yMax(rect.yMax);
            };

            auto ZoomAroundCenter = [this, &AnimateToRegion](const DevFloat scale) {
                const auto currentRect = Region.getRect();
                const auto xCenter = currentRect.xCenter();
                const auto yCenter = currentRect.yCenter();

                constexpr DevFloat MinHalfExtent = 1e-8;
                const auto halfWidth = std::max(static_cast<DevFloat>(0.5 * currentRect.GetWidth() * scale), MinHalfExtent);
                const auto halfHeight = std::max(static_cast<DevFloat>(0.5 * currentRect.GetHeight() * scale), MinHalfExtent);

                AnimateToRegion(RectR{
                    xCenter - halfWidth,
                    xCenter + halfWidth,
                    yCenter - halfHeight,
                    yCenter + halfHeight
                });
            };

            const auto popupName = AddUniqueIdToString(Title + Str(" window popup"));
            const bool bOpenPopupFromMouseCall = PopupOn && !POPUP_ON_MOUSE_CALL;
            PopupOn = false;

            constexpr float DefaultMargin = 8.0f;
            constexpr float DefaultStripPadding = 6.0f;
            constexpr float DefaultStripSpacing = 6.0f;
            constexpr float BaseToolbarButtonSize = 30.0f;
            constexpr float BaseRightButtonSize = 34.0f;

            const auto overlayStyle = GetGlobalOverlayControlsStyle();
            const auto Margin = std::max(overlayStyle.EdgeMargin, 0.0f);
            const auto StripPadding = std::max(overlayStyle.StripPadding, 0.0f);
            const auto StripSpacing = std::max(overlayStyle.StripSpacing, 0.0f);
            const auto toolbarScale = std::max(overlayStyle.ToolbarButtonSizeMultiplier, 0.25f);
            const auto rightScale = std::max(overlayStyle.RightButtonSizeMultiplier, 0.25f);
            const float ToolbarButtonSize = BaseToolbarButtonSize * toolbarScale;
            const float RightButtonSize = BaseRightButtonSize * rightScale;

            const float EffectiveMargin = Margin > 0.0f ? Margin : DefaultMargin;
            const float EffectiveStripPadding = StripPadding > 0.0f ? StripPadding : DefaultStripPadding;
            const float EffectiveStripSpacing = StripSpacing > 0.0f ? StripSpacing : DefaultStripSpacing;

            const Vector<FOverlayIconButton> ToolbarButtons = {
                {
                    "save",
                    []() { return FontAwesomeSolidIcons::Save; },
                    "Save plot as PNG",
                    [&SaveGraphToDisk]() { SaveGraphToDisk(); },
                    {}
                }
            };

            const Vector<FOverlayIconButton> RightButtons = {
                {
                    "zoom-in",
                    []() { return FontAwesomeSolidIcons::ZoomIn; },
                    "Zoom in",
                    [&ZoomAroundCenter]() { ZoomAroundCenter(0.85); },
                    {}
                },
                {
                    "zoom-out",
                    []() { return FontAwesomeSolidIcons::ZoomOut; },
                    "Zoom out",
                    [&ZoomAroundCenter]() { ZoomAroundCenter(1.15); },
                    {}
                },
                {
                    "aspect-11",
                    []() { return FontAwesomeSolidIcons::Aspect11; },
                    "Lock 1:1 unit aspect ratio",
                    [this]() {
                        LockUnitAspectRatio = !LockUnitAspectRatio;
                        if (LockUnitAspectRatio) {
                            EnforceUnitAspectRatio(true);
                        }
                    },
                    [this]() { return LockUnitAspectRatio; }
                },
                {
                    "fit-right",
                    []() { return FontAwesomeSolidIcons::Fit; },
                    "Fit graph to visible artists",
                    [this]() { ReviewGraphRanges(); },
                    {}
                },
                {
                    "auto-right",
                    []() { return FontAwesomeSolidIcons::AutoAdjust; },
                    "Toggle automatic fit",
                    [this]() { AutoReviewGraphRanges = !AutoReviewGraphRanges; },
                    [this]() { return AutoReviewGraphRanges; }
                },
                {
                    "ui-right",
                    [this]() { return ShowInterface ? FontAwesomeSolidIcons::HideInterface : FontAwesomeSolidIcons::ShowInterface; },
                    "Show or hide detail panel",
                    [this]() { ShowInterface = !ShowInterface; },
                    [this]() { return ShowInterface; }
                }
            };

            const auto RightStripSize = ComputeFloatingStripSize(
                RightButtons.size(),
                RightButtonSize,
                EffectiveStripSpacing,
                EffectiveStripPadding,
                true);

            const float panelWidth = std::clamp(
                static_cast<float>(Viewport.GetWidth()) * 0.30f,
                300.0f,
                460.0f);
            const float panelDefaultX = static_cast<float>(Viewport.xMax) - panelWidth - EffectiveMargin;

            const ImVec2 ToolbarPos{
                static_cast<float>(Viewport.xMin) + EffectiveMargin,
                static_cast<float>(Viewport.yMin) + EffectiveMargin
            };

            const float rightXMin = static_cast<float>(Viewport.xMin) + EffectiveMargin;
            const float rightXMax = std::max(
                rightXMin,
                static_cast<float>(Viewport.xMax) - EffectiveMargin - RightStripSize.x);
            float rightX = static_cast<float>(Viewport.xMax) - EffectiveMargin - RightStripSize.x;
            if (ShowInterface && overlayStyle.bRightStripAvoidDetailPanel) {
                rightX = panelDefaultX - EffectiveMargin - RightStripSize.x;
            }
            rightX = std::clamp(rightX, rightXMin, rightXMax);

            const float RightYMin = static_cast<float>(Viewport.yMin) + EffectiveMargin;
            const float RightYMax = static_cast<float>(Viewport.yMax) - EffectiveMargin - RightStripSize.y;
            const float RightYCentered =
                static_cast<float>(Viewport.yMin) + 0.5f * (static_cast<float>(Viewport.GetHeight()) - RightStripSize.y);
            const float RightYTopAligned = RightYMin + std::max(0.0f, overlayStyle.RightStripTopOffset);
            const float RightY = overlayStyle.bRightStripAlignTop ? RightYTopAligned : RightYCentered;

            const ImVec2 RightPos{
                rightX,
                std::clamp(RightY, RightYMin, std::max(RightYMin, RightYMax))
            };

            DrawFloatingIconStrip(
                AddUniqueIdToString("Plot Toolbar"),
                ToolbarPos,
                true,
                ToolbarButtonSize,
                EffectiveStripSpacing,
                EffectiveStripPadding,
                overlayStyle.ToolbarBackgroundAlpha,
                overlayStyle.bTransparentBackground,
                ToolbarButtons,
                [this, &popupName, &SaveGraphToDisk, bOpenPopupFromMouseCall]() {
                    if (bOpenPopupFromMouseCall) {
                        ImGui::OpenPopup(popupName.c_str());
                    }

                    if (ImGui::BeginPopup(popupName.c_str())) {
                        if (ImGui::MenuItem("Auto adjust", nullptr, AutoReviewGraphRanges)) {
                            AutoReviewGraphRanges = !AutoReviewGraphRanges;
                        }
                        if (ImGui::MenuItem("Show interface", nullptr, ShowInterface)) {
                            ShowInterface = !ShowInterface;
                        }
                        if (ImGui::MenuItem("Fit now")) {
                            ReviewGraphRanges();
                        }
                        if (ImGui::MenuItem("Lock 1:1 ratio", nullptr, LockUnitAspectRatio)) {
                            LockUnitAspectRatio = !LockUnitAspectRatio;
                            if (LockUnitAspectRatio) EnforceUnitAspectRatio(true);
                        }
                        if (ImGui::MenuItem("Save graph")) {
                            SaveGraphToDisk();
                        }
                        ImGui::EndPopup();
                    }
                });

            DrawFloatingIconStrip(
                AddUniqueIdToString("Plot Right Controls"),
                RightPos,
                true,
                RightButtonSize,
                EffectiveStripSpacing,
                EffectiveStripPadding,
                overlayStyle.RightBackgroundAlpha,
                overlayStyle.bTransparentBackground,
                RightButtons);

            if (!ShowInterface) return;

            const float panelTop = static_cast<float>(Viewport.yMin) + EffectiveMargin +
                std::max(0.0f, overlayStyle.RightStripTopOffset);
            const float panelHeight = std::max(
                200.0f,
                static_cast<float>(Viewport.yMax) - panelTop - EffectiveMargin);

            float panelX = panelDefaultX;
            if (RightPos.x + RightStripSize.x + EffectiveMargin > panelDefaultX) {
                panelX = RightPos.x - EffectiveMargin - panelWidth;
            }
            panelX = std::max(static_cast<float>(Viewport.xMin) + EffectiveMargin, panelX);

            const float panelY = panelTop;

            ImGui::SetNextWindowPos(ImVec2{panelX, panelY}, ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2{panelWidth, panelHeight}, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.78f);

            constexpr auto DetailFlags =
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings;

            if (ImGui::Begin(AddUniqueIdToString("Plot Detail").c_str(), nullptr, DetailFlags))
            {
                if (ImGui::CollapsingHeader("View", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Text("Auto adjust: %s", AutoReviewGraphRanges ? "On" : "Off");
                    ImGui::Text("Detail panel: %s", ShowInterface ? "Visible" : "Hidden");
                    ImGui::Text("1:1 lock: %s", LockUnitAspectRatio ? "On" : "Off");

                    const auto regionRect = Region.getRect();
                    ImGui::Text("x:[%.4g, %.4g]  y:[%.4g, %.4g]",
                        regionRect.xMin,
                        regionRect.xMax,
                        regionRect.yMin,
                        regionRect.yMax);
                    ImGui::Text("w=%.4g  h=%.4g", regionRect.GetWidth(), regionRect.GetHeight());
                    ImGui::TextDisabled("Use floating icon strips for fit/auto/save/zoom.");
                }

                if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto it = Content.begin(); it!=Content.end(); )
                    {
                        IN artie = it->second;
                        bool incrementIterator = true;

                        ImGui::PushID(artie.get());
                        if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
                            incrementIterator = !z_order_up(Content, it);
                        }
                        ImGui::SameLine();
                        if (ImGui::ArrowButton("##down", ImGuiDir_Down)) {
                            incrementIterator = !z_order_down(Content, it);
                        }
                        ImGui::SameLine();
                        bool visible = artie->IsVisible();
                        if (ImGui::Checkbox("##visible", &visible)) {
                            artie->SetVisibility(visible);
                        }
                        ImGui::SameLine();
                        ImGui::TextUnformatted(artie->GetLabel().c_str());
                        ImGui::SameLine();
                        ImGui::TextDisabled("(z=%i)", it->first);
                        ImGui::PopID();

                        if (incrementIterator) ++it;
                    }
                }

                if (ImGui::CollapsingHeader("Artist Controls", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    const auto availRegion = ImGui::GetContentRegionAvail();
                    ImGui::BeginChild(
                        AddUniqueIdToString(Title + " details").c_str(),
                        ImVec2{availRegion.x, 0},
                        ImGuiChildFlags_Border);

                    for (IN cont: Content) {
                        IN artie = cont.second;

                        if (artie->IsVisible() && artie->HasGUI()) {
                            if (ImGui::CollapsingHeader(AddUniqueIdToString(artie->GetLabel()).c_str())) {
                                artie->DrawGUI();
                            }
                        }
                    }

                    ImGui::EndChild();
                }
            }

            ImGui::End();
        };

        if (b_DrawGUI) DrawCall();
    }

    void FPlot2DWindow::EnforceUnitAspectRatio(const bool animate) {
        const auto viewport = GetViewport();
        const auto viewportWidth = static_cast<DevFloat>(std::max(1, viewport.GetWidth()));
        const auto viewportHeight = static_cast<DevFloat>(std::max(1, viewport.GetHeight()));
        const auto viewportAspect = viewportWidth / viewportHeight;
        if (viewportAspect <= 0.0) return;

        const auto current = Region.getRect();
        const auto xCenter = current.xCenter();
        const auto yCenter = current.yCenter();

        constexpr DevFloat MinHalfExtent = 1e-8;
        const auto halfWidth = std::max(static_cast<DevFloat>(0.5 * current.GetWidth()), MinHalfExtent);
        const auto halfHeight = std::max(halfWidth / viewportAspect, MinHalfExtent);

        const RectR target{
            xCenter - halfWidth,
            xCenter + halfWidth,
            yCenter - halfHeight,
            yCenter + halfHeight
        };

        if (animate) {
            Region.animate_xMin(target.xMin);
            Region.animate_xMax(target.xMax);
            Region.animate_yMin(target.yMin);
            Region.animate_yMax(target.yMax);
        } else {
            Region = target;
        }
    }

    void FPlot2DWindow::SetupOrtho() const {
        fix Viewport = GetViewport();
        fix CurrentStyle = FPlotThemeManager::GetCurrent();
        CurrentStyle->LabelsWriter->Reshape(Viewport.GetWidth(), Viewport.GetHeight());
        CurrentStyle->TicksWriter->Reshape(Viewport.GetWidth(), Viewport.GetHeight());
    }

    void FPlot2DWindow::ArtistsDraw() {
        SetupOrtho();

        for (const auto& Artist : Content | std::views::values)
        {
            if (Artist->IsVisible() && !Artist->Draw(*this))
            {
                Core::FLog::Error()
                << "In PlottingWindow \"" << Title
                << "\" while drawing Artist \"" << Artist->GetLabel() << "\"" << Core::FLog::Flush;
            }
        }
    }

    FAxisArtist &
    FPlot2DWindow::GetAxisArtist() { return AxisArtist; }

    auto FPlot2DWindow::GetWindowNumericId() const -> CountType { return Id; }

    auto FPlot2DWindow::GetStableWindowIdV2() const -> Str {
        return "legacy_plot_window_" + ToStr(Id);
    }

    auto FPlot2DWindow::GetWindowTitle() const -> const Str & { return Title; }

    auto FPlot2DWindow::GetArtists() const -> const ContentMap & { return Content; }

    auto FPlot2DWindow::GetArtists() -> ContentMap & { return Content; }

    auto FPlot2DWindow::TryGetArtistZOrder(const FArtist_ptr &pArtist, zOrder_t &zOrderOut) const -> bool {
        if (pArtist == nullptr) return false;

        const auto it = std::find_if(Content.begin(), Content.end(), [&](const auto &slot) {
            return slot.second == pArtist;
        });

        if (it == Content.end()) return false;
        zOrderOut = it->first;
        return true;
    }

    auto FPlot2DWindow::SetArtistZOrder(const FArtist_ptr &pArtist, const zOrder_t zOrder) -> bool {
        if (pArtist == nullptr) return false;

        const auto it = std::find_if(Content.begin(), Content.end(), [&](const auto &slot) {
            return slot.second == pArtist;
        });
        if (it == Content.end()) return false;

        const auto artist = it->second;
        Content.erase(it);
        Content.emplace(zOrder, artist);

        return true;
    }

    void
    FPlot2DWindow::SetAutoReviewGraphRanges(bool autoReview) { AutoReviewGraphRanges = autoReview; }

    auto FPlot2DWindow::GetAutoReviewGraphRanges() const -> bool { return AutoReviewGraphRanges; }

    void
    FPlot2DWindow::toggleShowInterface() { ShowInterface = !ShowInterface; }

    void
    FPlot2DWindow::RequireLabelOverlay(const Str& label, const TPointer<Graphics::PlotStyle>& style) const {
        const_cast<FLabelsArtist*>(&LabelsArtist)->add(label, style);
    }

    void FPlot2DWindow::TieRegion_xMaxMin(const Graphics::FPlot2DWindow &other) {
        Region.setReference_xMin(other.GetRegion().getReference_xMin());
        Region.setReference_xMax(other.GetRegion().getReference_xMax());
    }

    void FPlot2DWindow::TieRegion_yMaxMin(const Graphics::FPlot2DWindow &other) {
        Region.setReference_yMin(other.GetRegion().getReference_yMin());
        Region.setReference_yMax(other.GetRegion().getReference_yMax());
    }

    void FPlot2DWindow::RemoveArtists(const Vector<Graphics::FArtist_ptr> &artists) {
        for(const auto& artist : artists) RemoveArtist(artist);
    }

}
