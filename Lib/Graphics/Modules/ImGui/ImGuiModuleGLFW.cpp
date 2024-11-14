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

    static CallSet::InitContextCall Init = [](RawSystemWindowPointer raw_syswin_ptr) {
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)raw_syswin_ptr, DONT_INSTALL_CALLBACKS);
        ImGui_ImplOpenGL3_Init();

        return;
    };

    static CallSet::KillContextCall End = []() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    };

    static DrawCall Draw = [] {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    };

    static CallSet::NewFrameCall NewFrame = []() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
    };

    ImGuiModuleGLFW::ImGuiModuleGLFW() : ImGuiModule(CallSet{Init, End, Draw, NewFrame}) {  }

} // Core