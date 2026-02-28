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
#include "Graphics/SlabGraphics.h"

#include "Graphics/Window/WindowStyles.h"

namespace Slab::Graphics {

    // ** THEMES everybody **
    // https://github.com/ocornut/imgui/issues/707

    struct FImGuiThemeDescriptor
    {
        void (*ApplyStyle)() = nullptr;
        Str FontName;
    };

    Str CurrentTheme = "StudioSlab";
    std::map<Str, FImGuiThemeDescriptor> ImGuiThemes = {
            {"Native Light",      {SetColorThemeNativeLight, "Roboto"}},
            {"Native Dark",       {SetColorThemeNativeDark, "Roboto"}},
            {"Dark Red",          {SetColorThemeDarkRed, "Inconsolata"}},
            {"Dark",              {SetStyleDark, "Julia Mono"}},
            {"Light",             {SetStyleLight, "Roboto"}},
            {"StudioSlab",        {SetStyleStudioSlab, "Julia Mono"}},
            {"Scientific Slate",  {SetStyleScientificSlate, "DejaVu Sans"}},
            {"Scientific Paper",  {SetStyleScientificPaper, "New Computer Modern"}},
            {"Blueprint Night",   {SetStyleBlueprintNight, "Cousine"}},
    };

    auto ApplyTheme(const Str& themeName) -> bool
    {
        const auto it = ImGuiThemes.find(themeName);
        if (it == ImGuiThemes.end()) return false;
        if (it->second.ApplyStyle != nullptr) it->second.ApplyStyle();
        if (!it->second.FontName.empty()) {
            (void) SetImGuiThemeFont(it->second.FontName);
        }
        CurrentTheme = themeName;
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
        Vector<MainMenuLeafEntry> Entries;
        for (const auto& Name : ImGuiThemes | std::views::keys)
        {
            const bool bSelected = Name == CurrentTheme;
            Entries.emplace_back(MainMenuLeafEntry{Name, "", bSelected, true});
        }
        const MainMenuItem Item({"System", "ImGui"}, Entries, [](const Str& LeafEntry)
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

            const MainMenuItem fontItem({"System", "ImGui", "Font"}, fontEntries, [](const Str& leafEntry) {
                (void) SetImGuiThemeFont(leafEntry);
            });
            Context.AddMainMenuItem(fontItem);
        }
    }

    TPointer<FGUIContext> FImGuiModule::CreateContext(FOwnerPlatformWindow ParentSystemWindow) {
        auto NewContext = New<FImGuiContext>(ImplCallSet);

        if (!ApplyTheme(CurrentTheme)) {
            (void) ApplyTheme("StudioSlab");
        }

        Contexts.emplace_back(NewContext);

        return NewContext;
    }

} // Core
