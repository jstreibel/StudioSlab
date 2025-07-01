//
// Created by joao on 22/09/23.
//

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

    Str currentTheme = "Dark";
    std::map<Str, void(*)()> ColorThemes = {
            {"Native light", SetColorThemeNativeLight},
            {"Native dark",  SetColorThemeNativeDark},
            {"Dark red",     SetColorThemeDarkRed},
            {"Dark",         SetStyleDark},
            {"Light",        SetStyleLight},
            {"StudioSlab",   SetStyleStudioSlab}
    };

    FImGuiModule::FImGuiModule(FCallSet calls)
    : FGUIModule("ImGui")
    , CallSet(std::move(calls)) {
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

    Pointer<GUIContext> FImGuiModule::CreateContext(FOwnerPlatformWindow ParentSystemWindow) {
        auto NewContext = New<FImGuiContext>(CallSet);

        // Setup Dear ImGui style
        SetStyleStudioSlab();   // For sizes
        ColorThemes[currentTheme]();

        Contexts.emplace_back(NewContext);

        return NewContext;
    }

    // Context ImGuiModule::GetMainContext() {
    //     if(m_MainContext== nullptr){
    //         m_MainContext = DynamicPointerCast<SlabImGuiContext>(createContext());
    //         GetGraphicsBackend()->GetMainSystemWindow()->addEventListener(m_MainContext);
    //     }
//
    //     return m_MainContext;
    // }


} // Core