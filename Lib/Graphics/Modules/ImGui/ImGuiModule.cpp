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

    Str CurrentTheme = "Dark";
    std::map<Str, void(*)()> ColorThemes = {
            {"Native Light", SetColorThemeNativeLight},
            {"Native Dark",  SetColorThemeNativeDark},
            {"Dark Red",     SetColorThemeDarkRed},
            {"Dark",         SetStyleDark},
            {"Light",        SetStyleLight},
            {"StudioSlab",   SetStyleStudioSlab}
    };

    FImGuiModule::FImGuiModule(FImplementationCallSet calls)
    : FGUIModule("ImGui")
    , ImplCallSet(std::move(calls)) {
        IMGUI_CHECKVERSION();

        // m_MainContext = GetMainContext();
    }


    FImGuiModule* FImGuiModule::BuildModule() {
        const Str BackendImplName = Core::BackendManager::GetBackendName();

        if(BackendImplName == "Uninitialized")
        {
            throw NotImplementedException("ImGui must be built with a GUI backend already initialized.");
        }

        auto RawPlatformWindowPtr = GetGraphicsBackend()->GetMainSystemWindow()->GetRawPlatformWindowPointer();

        if(BackendImplName == "Headless")
            throw NotImplementedException("Headless ImGui module");
        if(BackendImplName == "GLFW")
        {
            return new ImGuiModuleGLFW(static_cast<GLFWwindow*>(RawPlatformWindowPtr));
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
        for (const auto& Name : ColorThemes | std::views::keys)
        {
            const bool bSelected = Name == CurrentTheme;
            Entries.emplace_back(MainMenuLeafEntry{Name, "", bSelected, true});
        }
        const MainMenuItem Item({"System", "ImGui"}, Entries, [](const Str& LeafEntry)
        {
            SetStyleStudioSlab();
            ColorThemes[LeafEntry]();
            CurrentTheme = LeafEntry;
        });

        Context.AddMainMenuItem(Item);
    }

    TPointer<FGUIContext> FImGuiModule::CreateContext(FOwnerPlatformWindow ParentSystemWindow) {
        auto NewContext = New<FImGuiContext>(ImplCallSet);

        // Setup Dear ImGui style
        SetStyleStudioSlab();   // For sizes
        ColorThemes[CurrentTheme]();

        Contexts.emplace_back(NewContext);

        return NewContext;
    }

} // Core