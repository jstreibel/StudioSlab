//
// Created by joao on 22/09/23.
//

#include <ranges>

#include "3rdParty/ImGui.h"

#ifndef IMGUI_USE_WCHAR32
#pragma error
#endif

#include "ImGuiColorAndStyles.h"

#include <filesystem>
#include <utility>

#include "ImGuiModule.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Tools/Log.h"

#include "ImGuiModuleGLFW.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/SlabGraphics.h"

#include "Graphics/Window/WindowStyles.h"

namespace Slab::Graphics {

    // ** THEMES everybody **
    // https://github.com/ocornut/imgui/issues/707

    struct FWindowThemeDescriptor
    {
        FColor PlatformBackgroundColor;
        FColor SlabWindowBackgroundColor;
        FColor TitlebarColor;
        FColor BorderColorInactive;
        FColor BorderColorActive;
    };

    struct FUnifiedThemeDescriptor
    {
        void (*ApplyImGuiStyle)() = nullptr;
        Str FontName;
        Str PlotThemeName;
        FWindowThemeDescriptor WindowTheme;
    };

    auto CurrentThemeName() -> Str & {
        static Str currentTheme = "Scientific Paper";
        return currentTheme;
    }

    auto UnifiedThemeRegistry() -> std::map<Str, FUnifiedThemeDescriptor> & {
        static std::map<Str, FUnifiedThemeDescriptor> unifiedThemes = {
            {"Native Light", {SetColorThemeNativeLight, "Roboto", "GUI Native Light", {
                FColor::FromHex("#F0EEE8"), FColor::FromHex("#F3EFE4"), FColor::FromHex("#E7E2D4"),
                FColor::FromHex("#B6AFA1"), FColor::FromHex("#D2CAB8")}}},
            {"Native Dark", {SetColorThemeNativeDark, "Roboto", "GUI Native Dark", {
                FColor::FromHex("#1D2025"), FColor::FromHex("#272B31"), FColor::FromHex("#323742"),
                FColor::FromHex("#5B6371"), FColor::FromHex("#7E899A")}}},
            {"Dark Red", {SetColorThemeDarkRed, "Inconsolata", "GUI Dark Red", {
                FColor::FromHex("#22181A"), FColor::FromHex("#2C1E21"), FColor::FromHex("#3A262A"),
                FColor::FromHex("#6C3F46"), FColor::FromHex("#9B4A56")}}},
            {"Dark", {SetStyleDark, "Julia Mono", "GUI Scientific Slate", {
                FColor::FromHex("#181D22"), FColor::FromHex("#222830"), FColor::FromHex("#2D3440"),
                FColor::FromHex("#566273"), FColor::FromHex("#748398")}}},
            {"Light", {SetStyleLight, "Roboto", "GUI Scientific Paper", {
                FColor::FromHex("#EDE9DE"), FColor::FromHex("#EFE9DB"), FColor::FromHex("#DFD8C7"),
                FColor::FromHex("#AFA38D"), FColor::FromHex("#C2B59D")}}},
            {"StudioSlab", {SetStyleStudioSlab, "Julia Mono", "GUI StudioSlab", {
                FColor::FromHex("#23252A"), FColor::FromHex("#2E3239"), FColor::FromHex("#383D46"),
                FColor::FromHex("#5A6371"), FColor::FromHex("#788395")}}},
            {"Scientific Slate", {SetStyleScientificSlate, "DejaVu Sans", "GUI Scientific Slate", {
                FColor::FromHex("#121A22"), FColor::FromHex("#1A2530"), FColor::FromHex("#23313E"),
                FColor::FromHex("#406178"), FColor::FromHex("#5D89A8")}}},
            {"Scientific Paper", {SetStyleScientificPaper, "New Computer Modern", "GUI Scientific Paper", {
                FColor::FromHex("#EEE8DB"), FColor::FromHex("#F2ECE0"), FColor::FromHex("#E2D8C5"),
                FColor::FromHex("#B3A286"), FColor::FromHex("#CCB792")}}},
            {"Blueprint Night", {SetStyleBlueprintNight, "Cousine", "GUI Blueprint Night", {
                FColor::FromHex("#071220"), FColor::FromHex("#0A1A2C"), FColor::FromHex("#12304A"),
                FColor::FromHex("#25587D"), FColor::FromHex("#3A7CA9")}}},
        };
        return unifiedThemes;
    }

    auto ApplyWindowTheme(const FWindowThemeDescriptor& windowTheme) -> void
    {
        WindowStyle::PlatformWindow_BackgroundColor = windowTheme.PlatformBackgroundColor;
        WindowStyle::WindowBGColor = windowTheme.SlabWindowBackgroundColor;
        WindowStyle::titlebar_color = windowTheme.TitlebarColor;
        WindowStyle::windowBorderColor_inactive = windowTheme.BorderColorInactive;
        WindowStyle::windowBorderColor_active = windowTheme.BorderColorActive;
    }

    auto ApplyTheme(const Str& themeName) -> bool
    {
        auto &themeRegistry = UnifiedThemeRegistry();
        const auto it = themeRegistry.find(themeName);
        if (it == themeRegistry.end()) return false;

        if (it->second.ApplyImGuiStyle != nullptr) it->second.ApplyImGuiStyle();
        if (!it->second.FontName.empty()) {
            (void) SetImGuiThemeFont(it->second.FontName);
        }
        if (!it->second.PlotThemeName.empty()) {
            const bool plotThemeSet = FPlotThemeManager::SetTheme(it->second.PlotThemeName);
            if (!plotThemeSet) {
                Core::FLog::Warning()
                    << "Could not map unified theme '" << themeName
                    << "' to plot theme '" << it->second.PlotThemeName << "'."
                    << Core::FLog::Flush;
            }
        }
        ApplyWindowTheme(it->second.WindowTheme);

        CurrentThemeName() = themeName;

        return true;
    }

    FImGuiModule::FImGuiModule(FImplementationCallSet calls)
    : FGUIModule("ImGui")
    , ImplCallSet(std::move(calls)) {
        IMGUI_CHECKVERSION();

        // m_MainContext = GetMainContext();
    }


    FImGuiModule* FImGuiModule::BuildModule() {
        const Str BackendImplName = Core::FBackendManager::GetBackendName();

        if(BackendImplName == "Uninitialized")
        {
            throw NotImplementedException("ImGui must be built with a GUI backend already initialized.");
        }

        auto RawPlatformWindowPtr = GetGraphicsBackend()->GetMainSystemWindow()->GetRawPlatformWindowPointer();

        if(BackendImplName == "Headless")
            throw NotImplementedException("Headless ImGui module");
        if(BackendImplName == "GLFW")
        {
            return new FImGuiModuleGLFW(static_cast<GLFWwindow*>(RawPlatformWindowPtr));
        }
        if(BackendImplName == "SFML")
            throw NotImplementedException("SFML ImGui module");
        if(BackendImplName == "VTK")
            throw NotImplementedException("VTK ImGui module");
        if(BackendImplName == "GLUT")
        {
            // return new ImGuiModuleGLUT();
            NOT_IMPLEMENTED;
        }

        NOT_IMPLEMENTED;
    }

    void FImGuiModule::SetupOptionalMenuItems(FImGuiContext& Context)
    {
        const auto &themes = UnifiedThemeRegistry();
        const auto &currentTheme = CurrentThemeName();

        Vector<MainMenuLeafEntry> Entries;
        for (const auto& Name : themes | std::views::keys)
        {
            const bool bSelected = Name == currentTheme;
            Entries.emplace_back(MainMenuLeafEntry{Name, "", bSelected, true});
        }
        const MainMenuItem Item({"System", "Theme"}, Entries, [](const Str& LeafEntry)
        {
            (void) ApplyTheme(LeafEntry);
        });

        Context.AddMainMenuItem(Item);

        const auto& fontNames = ListImGuiThemeFonts();
        if (!fontNames.empty()) {
            Vector<MainMenuLeafEntry> fontEntries;
            const auto currentFont = GetCurrentImGuiThemeFont();
            for (const auto& fontName : fontNames) {
                const bool bSelected = fontName == currentFont;
                fontEntries.emplace_back(MainMenuLeafEntry{fontName, "", bSelected, true});
            }

            const MainMenuItem fontItem({"System", "Theme", "Font"}, fontEntries, [](const Str& leafEntry) {
                (void) SetImGuiThemeFont(leafEntry);
            });
            Context.AddMainMenuItem(fontItem);
        }
    }

    TPointer<FGUIContext> FImGuiModule::CreateContext(FOwnerPlatformWindow ParentSystemWindow) {
        auto NewContext = New<FImGuiContext>(ImplCallSet);

        if (!ApplyTheme(CurrentThemeName())) {
            (void) ApplyTheme("StudioSlab");
        }

        Contexts.emplace_back(NewContext);

        return NewContext;
    }

} // Core
