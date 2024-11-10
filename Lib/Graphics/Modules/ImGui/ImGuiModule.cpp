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
#include "Core/Tools/Resources.h"
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
    : GraphicsModule("ImGui")
    , call_set(std::move(calls)) {
        IMGUI_CHECKVERSION();

        m_MainContext = createContext();

    }

    ImGuiModule::~ImGuiModule() = default;


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

    void ImGuiModule::Update() {
        m_MainContext->NewFrame();

        if (showDemos)
            ImGui::ShowDemoWindow();

        ImGui::BeginMainMenuBar();
        {
            if(ImGui::BeginMenu("Menu")) {
                if(ImGui::MenuItem("Quit", "Alt+F4")) Core::BackendManager::GetBackend()->terminate();

                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Style")) {
                if(ImGui::BeginMenu("ImGui")) {
                    for(const auto& theme : colorThemes) {
                        if (ImGui::MenuItem(theme.first.c_str(), nullptr,
                                            theme.first==currentTheme)) {
                            theme.second();
                            currentTheme = theme.first;
                        }
                    }
                    ImGui::MenuItem("---", nullptr, false, false);
                    ImGui::MenuItem("Show ImGui demos", nullptr, &showDemos);

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        m_MainContext->notifyRender();
    }

    Pointer<SlabImGuiContext> ImGuiModule::createContext() {
        auto new_context = New<SlabImGuiContext>(call_set);

        // Setup Dear ImGui style
        SetStyleStudioSlab();   // For sizes
        colorThemes[currentTheme]();

        contexts.emplace_back(new_context);

        return new_context;
    }

    Context ImGuiModule::GetMainContext() {
        return m_MainContext;
    }


} // Core