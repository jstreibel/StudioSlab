//
// Created by joao on 27/09/23.
//

#include "ImGuiModuleGLFW.h"
#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Don't touch
fix DONT_INSTALL_CALLBACKS = false;
fix PRIORITIZE_ME = true;

namespace Slab::Graphics {

    static FCallSet::InitContextCall Init = [](const FCallSet& Call) {
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)Call.r_SystemWindow, DONT_INSTALL_CALLBACKS);
        ImGui_ImplOpenGL3_Init();

        return;
    };

    static FCallSet::KillContextCall End = []() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    };

    static FDrawCall Draw = [] {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    };

    static FCallSet::NewFrameCall NewFrame = []() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
    };

    ImGuiModuleGLFW::ImGuiModuleGLFW(GLFWwindow* Window)
    : FImGuiModule(FCallSet{Window, Init, End, Draw, NewFrame}) {  }

} // Core