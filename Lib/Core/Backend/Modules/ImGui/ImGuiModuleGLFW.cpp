//
// Created by joao on 27/09/23.
//

#include "ImGuiModuleGLFW.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/GLFW/GLFWBackend.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Don't touch
fix DONT_INSTALL_CALLBACKS = false;

namespace Core {
    ImGuiModuleGLFW::ImGuiModuleGLFW() : ImGuiModule(GLFW) {

        auto backend = dynamic_cast<GLFWBackend*>(&Core::BackendManager::GetGUIBackend());
        if(backend == nullptr)
            throw StudioSlabRuntimeBackendInconsistency("while instantiating ImGui module (GLFW implementation)");

        auto &window = backend->getGLFWWindow();
        ImGui_ImplGlfw_InitForOpenGL(&window, DONT_INSTALL_CALLBACKS);
        ImGui_ImplOpenGL3_Init();

        ImGuiModule::finishInitialization();

        backend->addGLFWListener(this);
    }

    ImGuiModuleGLFW::~ImGuiModuleGLFW() {
        // ImGui_ImplGlfw_Shutdown();
    }

    void ImGuiModuleGLFW::beginRender() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        //ImGui::NewFrame();

        ImGuiModule::beginRender();
    }



    bool ImGuiModuleGLFW::KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

        if(!ImGui::GetIO().WantCaptureKeyboard) {
            if(key == GLFW_KEY_D){
                showDemos = !showDemos;

                return true;
            }
        }

        return false;
    }

    bool ImGuiModuleGLFW::MouseMotion(GLFWwindow *window, double xpos, double ypos) {
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

        return ImGui::GetIO().WantCaptureMouse;
    }

    void ImGuiModuleGLFW::CursorEnter(GLFWwindow *window, int entered) {
        ImGui_ImplGlfw_CursorEnterCallback(window, entered);
    }

    bool ImGuiModuleGLFW::MouseButton(GLFWwindow *window, int button, int action, int mods) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiModuleGLFW::MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiModuleGLFW::DroppedFiles(GLFWwindow *window, int count, const char **paths) {
        return GLFWListener::DroppedFiles(window, count, paths);
    }

    void ImGuiModuleGLFW::ScreenReshape(GLFWwindow *window, int width, int height) {
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float) width, (float) height);
    }

    void ImGuiModuleGLFW::Render(GLFWwindow *window) { if (showDemos) ImGui::ShowDemoWindow(); }


} // Core