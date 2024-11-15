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

#include "Graphics/Window/WindowStyles.h"

namespace Slab::Graphics {

    // ** THEMES everybody **
    // https://github.com/ocornut/imgui/issues/707

    Str currentTheme = "Dark";
    std::map<Str, void(*)()> colorThemes = {
            {"Native light", SetColorThemeNativeLight},
            {"Native dark",  SetColorThemeNativeDark},
            {"Dark red",     SetColorThemeDarkRed},
            {"Dark",         SetStyleDark},
            {"Light",        SetStyleLight},
            {"StudioSlab",   SetStyleStudioSlab}
    };

    ImGuiModule::ImGuiModule(CallSet calls)
    : GUIModule("ImGui", nullptr)
    , call_set(std::move(calls)) {
        IMGUI_CHECKVERSION();

        // m_MainContext = GetMainContext();
    }


    ImGuiModule* ImGuiModule::BuildModule() {
        Str backendImpl = Core::BackendManager::GetBackendName();

        if(backendImpl == "Uninitialized")
                throw NotImplementedException("ImGui must be built with a GUI backend already initialized.");
        else if(backendImpl == "Headless")
                throw NotImplementedException("Headless ImGui module");
        else if(backendImpl == "GLFW")
                return new ImGuiModuleGLFW();
        else if(backendImpl == "SFML")
                throw NotImplementedException("SFML ImGui module");
        else if(backendImpl == "VTK")
                throw NotImplementedException("VTK ImGui module");
        // else if(backendImpl == "GLUT")
        //         return new ImGuiModuleGLUT();

        NOT_IMPLEMENTED;
    }

    Pointer<GUIContext> ImGuiModule::createContext(ParentSystemWindow parentSystemWindow) {
        auto new_context = New<SlabImGuiContext>(parentSystemWindow, call_set);

        // Setup Dear ImGui style
        SetStyleStudioSlab();   // For sizes
        colorThemes[currentTheme]();

        contexts.emplace_back(new_context);

        return new_context;
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