//
// Created by joao on 3/10/23.
//

#include "ImGuiColorAndStyles.h"

#include <filesystem>
#include <map>

#include "Core/Tools/Log.h"
#include "Core/Tools/Resources.h"
#include "Utils/Exception.h"

namespace Slab::Graphics {
    namespace {

        struct FThemeFontSpec
        {
            Str Label;
            Str Path;
        };

        std::map<Str, ImFont*> ThemeFonts;
        Vector<Str> ThemeFontLabels;
        Str CurrentThemeFont = "";

        auto BuildGlyphRanges() -> ImVector<ImWchar>
        {
            static const ImWchar WideCharacterRanges[] =
                {
                    0x0020, 0x007F, // Basic Latin
                    0x00B0, 0x00BF, // Superscript / subscript
                    0x0391, 0x03C9, // Greek
                    0x03D0, 0x03F6,
                    0x2000, 0x2311, // math symbols
                    0x1D400, 0x1D7FF, // Mathematical alphanumeric symbols
                    0,
                };

            ImFontGlyphRangesBuilder glyphRangesBuilder;
            glyphRangesBuilder.AddRanges(WideCharacterRanges);
            glyphRangesBuilder.AddChar(ImWchar(0x1D62)); // subscript i
            glyphRangesBuilder.AddChar(ImWchar(0x21A6)); // mapsto

            ImVector<ImWchar> ranges;
            glyphRangesBuilder.BuildRanges(&ranges);
            return ranges;
        }

        auto ResetStyleDarkBase() -> ImGuiStyle
        {
            ImGuiStyle style;
            ImGui::StyleColorsDark(&style);
            return style;
        }

        auto ResetStyleLightBase() -> ImGuiStyle
        {
            ImGuiStyle style;
            ImGui::StyleColorsLight(&style);
            return style;
        }

        auto ApplyDeterministicMetrics(ImGuiStyle &style, const bool compact = false) -> void
        {
            const float sizeFactor = compact ? 0.90f : 1.0f;

            style.Alpha = 1.0f;
            style.DisabledAlpha = 0.58f;

            style.WindowPadding = ImVec2(10.0f * sizeFactor, 8.0f * sizeFactor);
            style.WindowRounding = 4.0f;
            style.WindowBorderSize = 1.0f;
            style.WindowMinSize = ImVec2(120.0f, 96.0f);
            style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
            style.WindowMenuButtonPosition = ImGuiDir_Left;

            style.ChildRounding = 4.0f;
            style.ChildBorderSize = 1.0f;
            style.PopupRounding = 4.0f;
            style.PopupBorderSize = 1.0f;

            style.FramePadding = ImVec2(8.0f * sizeFactor, 5.0f * sizeFactor);
            style.FrameRounding = 3.0f;
            style.FrameBorderSize = 1.0f;

            style.ItemSpacing = ImVec2(9.0f * sizeFactor, 6.0f * sizeFactor);
            style.ItemInnerSpacing = ImVec2(7.0f * sizeFactor, 4.0f * sizeFactor);
            style.CellPadding = ImVec2(7.0f * sizeFactor, 4.0f * sizeFactor);
            style.TouchExtraPadding = ImVec2(0.0f, 0.0f);

            style.IndentSpacing = 22.0f * sizeFactor;
            style.ColumnsMinSpacing = 8.0f;
            style.ScrollbarSize = 15.0f * sizeFactor;
            style.ScrollbarRounding = 4.0f;
            style.GrabMinSize = 11.0f * sizeFactor;
            style.GrabRounding = 3.0f;
            style.LogSliderDeadzone = 4.0f;

            style.TabRounding = 4.0f;
            style.TabBorderSize = 1.0f;
            style.TabMinWidthForCloseButton = 0.0f;
            style.TabBarBorderSize = 1.0f;
            style.TabBarOverlineSize = 2.0f;
            style.TableAngledHeadersAngle = 35.0f;
            style.TableAngledHeadersTextAlign = ImVec2(0.5f, 0.0f);

            style.ColorButtonPosition = ImGuiDir_Right;
            style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
            style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

            style.SeparatorTextBorderSize = 1.0f;
            style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
            style.SeparatorTextPadding = ImVec2(20.0f, style.FramePadding.y);

            style.DisplayWindowPadding = ImVec2(10.0f, 10.0f);
            style.DisplaySafeAreaPadding = ImVec2(3.0f, 3.0f);
            style.DockingSeparatorSize = 2.0f;
            style.MouseCursorScale = 1.0f;

            style.AntiAliasedLines = true;
            style.AntiAliasedLinesUseTex = true;
            style.AntiAliasedFill = true;
            style.CurveTessellationTol = 1.25f;
            style.CircleTessellationMaxError = 0.30f;

            style.HoverStationaryDelay = 0.10f;
            style.HoverDelayShort = 0.12f;
            style.HoverDelayNormal = 0.35f;
            style.HoverFlagsForTooltipMouse = ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort;
            style.HoverFlagsForTooltipNav = ImGuiHoveredFlags_DelayNormal;
        }

        auto SetScientificSlatePalette(ImVec4 *colors) -> void
        {
            colors[ImGuiCol_Text] = ImVec4(0.86f, 0.90f, 0.93f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.50f, 0.55f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.11f, 0.14f, 0.98f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.12f, 0.15f, 0.98f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.13f, 0.16f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.23f, 0.28f, 0.33f, 0.95f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.17f, 0.21f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17f, 0.24f, 0.30f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.29f, 0.36f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.10f, 0.12f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.18f, 0.23f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.12f, 0.15f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.13f, 0.20f, 0.27f, 0.75f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.30f, 0.38f, 0.78f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.22f, 0.37f, 0.46f, 0.90f);
            colors[ImGuiCol_Button] = ImVec4(0.14f, 0.22f, 0.30f, 0.72f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.31f, 0.40f, 0.86f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.41f, 0.52f, 0.92f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.37f, 0.84f, 0.89f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.33f, 0.73f, 0.79f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.42f, 0.88f, 0.96f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.16f, 0.21f, 0.92f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.30f, 0.38f, 0.94f);
            colors[ImGuiCol_TabActive] = ImVec4(0.17f, 0.25f, 0.32f, 1.00f);
            colors[ImGuiCol_Separator] = ImVec4(0.24f, 0.30f, 0.37f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.73f, 0.80f, 0.35f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.36f, 0.73f, 0.80f, 0.70f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.36f, 0.73f, 0.80f, 0.95f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.46f, 0.60f, 0.35f);
            colors[ImGuiCol_DockingPreview] = ImVec4(0.32f, 0.70f, 0.76f, 0.62f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.17f, 0.21f, 0.26f, 0.30f);
        }

        auto SetScientificPaperPalette(ImVec4 *colors) -> void
        {
            colors[ImGuiCol_Text] = ImVec4(0.17f, 0.15f, 0.13f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.47f, 0.42f, 0.38f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.92f, 0.88f, 0.98f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.92f, 0.90f, 0.86f, 0.95f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.97f, 0.95f, 0.92f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.58f, 0.53f, 0.47f, 0.70f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.89f, 0.86f, 0.81f, 0.85f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.83f, 0.79f, 0.72f, 0.90f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.74f, 0.66f, 0.95f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.82f, 0.78f, 0.71f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.76f, 0.70f, 0.61f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.87f, 0.84f, 0.78f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.72f, 0.67f, 0.58f, 0.65f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.67f, 0.60f, 0.48f, 0.74f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.61f, 0.54f, 0.41f, 0.82f);
            colors[ImGuiCol_Button] = ImVec4(0.69f, 0.62f, 0.50f, 0.58f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.62f, 0.54f, 0.40f, 0.74f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.52f, 0.44f, 0.30f, 0.86f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.31f, 0.52f, 0.65f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.61f, 0.74f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.33f, 0.51f, 0.64f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.82f, 0.78f, 0.70f, 0.85f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.74f, 0.68f, 0.58f, 0.90f);
            colors[ImGuiCol_TabActive] = ImVec4(0.68f, 0.61f, 0.49f, 1.00f);
            colors[ImGuiCol_Separator] = ImVec4(0.60f, 0.54f, 0.46f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.48f, 0.62f, 0.72f, 0.38f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.48f, 0.62f, 0.72f, 0.70f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.48f, 0.62f, 0.72f, 0.95f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.48f, 0.62f, 0.72f, 0.28f);
            colors[ImGuiCol_DockingPreview] = ImVec4(0.42f, 0.58f, 0.68f, 0.45f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.80f, 0.75f, 0.67f, 0.19f);
        }

        auto SetBlueprintNightPalette(ImVec4 *colors) -> void
        {
            colors[ImGuiCol_Text] = ImVec4(0.78f, 0.91f, 1.00f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.46f, 0.60f, 0.73f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.08f, 0.15f, 0.98f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.04f, 0.09f, 0.17f, 0.98f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.10f, 0.19f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.17f, 0.38f, 0.58f, 0.72f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.16f, 0.25f, 0.95f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.11f, 0.22f, 0.34f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.14f, 0.28f, 0.41f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.08f, 0.14f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.20f, 0.31f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.05f, 0.11f, 0.18f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.11f, 0.25f, 0.38f, 0.76f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.14f, 0.33f, 0.50f, 0.84f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.17f, 0.41f, 0.60f, 0.92f);
            colors[ImGuiCol_Button] = ImVec4(0.12f, 0.27f, 0.41f, 0.72f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.37f, 0.55f, 0.84f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.45f, 0.66f, 0.94f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.49f, 0.86f, 1.00f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.46f, 0.79f, 0.98f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.90f, 1.00f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.17f, 0.25f, 0.90f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.30f, 0.43f, 0.94f);
            colors[ImGuiCol_TabActive] = ImVec4(0.13f, 0.26f, 0.37f, 1.00f);
            colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.39f, 0.55f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.84f, 1.00f, 0.38f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.50f, 0.84f, 1.00f, 0.73f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.84f, 1.00f, 0.97f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.21f, 0.47f, 0.66f, 0.36f);
            colors[ImGuiCol_DockingPreview] = ImVec4(0.41f, 0.76f, 0.95f, 0.56f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.10f, 0.19f, 0.28f, 0.28f);
        }

        auto MergeFontAwesomeIntoThemeFont(
            ImGuiIO &io,
            const Str& iconFontPath,
            ImFont* baseFont,
            const float baseFontSizePixels) -> bool
        {
            if (baseFont == nullptr) return false;
            if (!std::filesystem::exists(iconFontPath)) return false;

            static constexpr ImWchar IconRanges[] = {0xF000, 0xF8FF, 0};

            ImFontConfig iconConfig;
            iconConfig.MergeMode = true;
            iconConfig.DstFont = baseFont;
            iconConfig.PixelSnapH = true;
            iconConfig.GlyphMinAdvanceX = baseFontSizePixels * 0.85f;
            iconConfig.GlyphOffset = ImVec2{0.0f, 0.0f};

            return io.Fonts->AddFontFromFileTTF(
                iconFontPath.c_str(),
                baseFontSizePixels * 0.95f,
                &iconConfig,
                IconRanges) != nullptr;
        }

        auto RegisterThemeFonts(float fontSizePixels) -> void
        {
            ImGuiIO &io = ImGui::GetIO();
            io.Fonts->Clear();
            ThemeFonts.clear();
            ThemeFontLabels.clear();

            const auto ranges = BuildGlyphRanges();
            const auto iconFontPath = Core::Resources::BuiltinFonts::FontAwesome_Solid();
            const bool haveIconFont = std::filesystem::exists(iconFontPath);

            if (!haveIconFont)
            {
                Core::FLog::Warning() << "ImGui icon font not found at " << iconFontPath
                    << ". Overlay icon buttons may not render as expected." << Core::FLog::Flush;
            }
            bool mergedIconIntoAnyFont = false;

            const Vector<FThemeFontSpec> fonts = {
                {"Julia Mono", Core::Resources::BuiltinFonts::JuliaMono_Regular()},
                {"DejaVu Sans", Core::Resources::BuiltinFonts::DejaVuSans()},
                {"Roboto", Core::Resources::BuiltinFonts::RobotoRegular()},
                {"Inconsolata", Core::Resources::BuiltinFonts::Inconsolata_Regular()},
                {"New Computer Modern", Core::Resources::BuiltinFonts::NewComputerModern10_Regular()},
                {"Latin Modern Roman", Core::Resources::BuiltinFonts::LatinModern10_Regular()},
                {"Droid Sans", Core::Resources::BuiltinFonts::DroidSans()},
                {"Cousine", Core::Resources::BuiltinFonts::CousineRegular()},
            };

            for (const auto &[label, path] : fonts)
            {
                if (!std::filesystem::exists(path))
                {
                    Core::FLog::Warning() << "ImGui theme font '" << label
                        << "' not found at " << path << Core::FLog::Flush;
                    continue;
                }

                ImFontConfig fontConfig;
                fontConfig.OversampleH = 3;
                fontConfig.OversampleV = 3;
                fontConfig.PixelSnapH = false;

                auto *font = io.Fonts->AddFontFromFileTTF(path.c_str(), fontSizePixels, &fontConfig, ranges.Data);
                if (font == nullptr)
                {
                    Core::FLog::Warning() << "ImGui could not load font '" << label
                        << "' from " << path << Core::FLog::Flush;
                    continue;
                }

                if (haveIconFont)
                {
                    const bool mergedIcon = MergeFontAwesomeIntoThemeFont(io, iconFontPath, font, fontSizePixels);
                    mergedIconIntoAnyFont = mergedIconIntoAnyFont || mergedIcon;

                    if (!mergedIcon)
                    {
                        Core::FLog::Warning() << "Could not merge Font Awesome icon font into '" << label << "'."
                            << Core::FLog::Flush;
                    }
                }

                ThemeFonts[label] = font;
                ThemeFontLabels.emplace_back(label);
            }

            if (ThemeFontLabels.empty())
            {
                throw Exception("ImGui theme font atlas is empty.");
            }

            const Str defaultFont = ThemeFonts.contains("Julia Mono")
                ? "Julia Mono"
                : ThemeFontLabels.front();

            io.FontDefault = ThemeFonts[defaultFont];
            CurrentThemeFont = defaultFont;
            io.Fonts->Build();

            Core::FLog::Info() << "ImGui theme fonts loaded: " << ToStr(ThemeFontLabels)
                << ". Default: " << CurrentThemeFont
                << ". FontAwesome merged: " << (mergedIconIntoAnyFont ? "yes" : "no")
                << Core::FLog::Flush;
        }
    } // namespace

    void BuildImGuiThemeFontAtlas(const float fontSizePixels)
    {
        RegisterThemeFonts(fontSizePixels);
    }

    bool SetImGuiThemeFont(const Str &fontName)
    {
        const auto it = ThemeFonts.find(fontName);
        if (it == ThemeFonts.end()) return false;

        ImGuiIO &io = ImGui::GetIO();
        io.FontDefault = it->second;
        CurrentThemeFont = fontName;
        return true;
    }

    auto ListImGuiThemeFonts() -> const Vector<Str>&
    {
        return ThemeFontLabels;
    }

    auto GetCurrentImGuiThemeFont() -> Str
    {
        return CurrentThemeFont;
    }

    void SetColorThemeNativeLight()
    {
        auto style = ResetStyleLightBase();
        ApplyDeterministicMetrics(style);
        ImGui::GetStyle() = style;
    }

    void SetColorThemeNativeDark()
    {
        auto style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        ImGui::GetStyle() = style;
    }

    void SetColorThemeDarkRed()
    {
        auto style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        auto *colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.04f, 0.05f, 0.98f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.05f, 0.06f, 0.98f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.06f, 0.07f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.38f, 0.17f, 0.19f, 0.78f);
        colors[ImGuiCol_Header] = ImVec4(0.33f, 0.14f, 0.17f, 0.72f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.43f, 0.18f, 0.22f, 0.82f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.51f, 0.22f, 0.27f, 0.92f);
        colors[ImGuiCol_Button] = ImVec4(0.38f, 0.14f, 0.18f, 0.64f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.19f, 0.24f, 0.82f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.57f, 0.23f, 0.29f, 0.94f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.49f, 0.43f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.88f, 0.37f, 0.33f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.98f, 0.52f, 0.42f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.17f, 0.08f, 0.10f, 0.89f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.43f, 0.18f, 0.22f, 0.90f);
        colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.13f, 0.16f, 0.96f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.88f, 0.36f, 0.33f, 0.52f);

        ImGui::GetStyle() = style;
    }

    void SetStyleDark()
    {
        auto style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        SetScientificSlatePalette(style.Colors);
        ImGui::GetStyle() = style;
    }

    void SetStyleLight()
    {
        auto style = ResetStyleLightBase();
        ApplyDeterministicMetrics(style, true);
        SetScientificPaperPalette(style.Colors);
        style.WindowRounding = 3.0f;
        style.FrameRounding = 2.0f;
        style.TabRounding = 3.0f;
        ImGui::GetStyle() = style;
    }

    void SetStyleStudioSlab()
    {
        auto style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        auto *colors = style.Colors;

        colors[ImGuiCol_Text] = ImVec4(0.83f, 0.86f, 0.88f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.52f, 0.56f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.11f, 0.13f, 0.98f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.12f, 0.14f, 0.98f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.12f, 0.14f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.26f, 0.29f, 0.33f, 0.80f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.22f, 0.26f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.23f, 0.28f, 0.34f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.19f, 0.24f, 0.30f, 0.68f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.31f, 0.38f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.29f, 0.37f, 0.45f, 0.90f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.26f, 0.33f, 0.60f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.34f, 0.43f, 0.82f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.31f, 0.42f, 0.52f, 0.92f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.57f, 0.82f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.77f, 0.95f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.63f, 0.86f, 1.00f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.15f, 0.18f, 0.92f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.33f, 0.42f, 0.92f);
        colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.24f, 0.30f, 0.98f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.52f, 0.77f, 0.95f, 0.54f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.17f, 0.20f, 0.24f, 0.28f);

        style.WindowPadding = ImVec2(11.0f, 8.0f);
        style.FramePadding = ImVec2(9.0f, 6.0f);
        style.ItemSpacing = ImVec2(10.0f, 7.0f);
        style.SeparatorTextPadding = ImVec2(18.0f, style.FramePadding.y);

        ImGui::GetStyle() = style;
    }

    void SetStyleScientificSlate()
    {
        auto style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        SetScientificSlatePalette(style.Colors);
        ImGui::GetStyle() = style;
    }

    void SetStyleScientificPaper()
    {
        auto style = ResetStyleLightBase();
        ApplyDeterministicMetrics(style, true);
        SetScientificPaperPalette(style.Colors);
        style.WindowRounding = 2.0f;
        style.ChildRounding = 2.0f;
        style.PopupRounding = 2.0f;
        style.FrameRounding = 2.0f;
        style.TabRounding = 2.0f;
        ImGui::GetStyle() = style;
    }

    void SetStyleBlueprintNight()
    {
        auto style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        SetBlueprintNightPalette(style.Colors);
        style.WindowRounding = 2.0f;
        style.ChildRounding = 2.0f;
        style.FrameRounding = 2.0f;
        style.TabRounding = 2.0f;
        ImGui::GetStyle() = style;
    }

} // Slab::Graphics
