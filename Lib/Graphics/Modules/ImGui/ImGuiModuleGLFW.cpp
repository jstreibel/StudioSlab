//
// Created by joao on 27/09/23.
//

#include "ImGuiModuleGLFW.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Don't touch
fix DONT_INSTALL_CALLBACKS = false;
fix PRIORITIZE_ME = true;

namespace Slab::Graphics {
    static auto initializer = [](ImGuiContext*) {
        // ImGuiModule::initializeContext(context);

        auto backend = DynamicPointerCast<GLFWBackend>(Core::BackendManager::GetBackend());
        if(backend == nullptr)
            throw StudioSlabRuntimeBackendInconsistency("while instantiating ImGui module (GLFW implementation)");

        auto &window = backend->getGLFWWindow();
        ImGui_ImplGlfw_InitForOpenGL(&window, DONT_INSTALL_CALLBACKS);
        ImGui_ImplOpenGL3_Init();
    };

    ImGuiModuleGLFW::ImGuiModuleGLFW() : ImGuiModule(initializer) {
        auto backend = DynamicPointerCast<GLFWBackend>(Core::BackendManager::GetBackend());
        if(backend == nullptr)
            throw StudioSlabRuntimeBackendInconsistency("while instantiating ImGui module (GLFW implementation)");

        static auto myReference = Naked(*this);
        backend->addGLFWListener(myReference, PRIORITIZE_ME);
    }

    ImGuiModuleGLFW::~ImGuiModuleGLFW() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }


    bool ImGuiModuleGLFW::KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

        return ImGui::GetIO().WantCaptureKeyboard;
    }

    bool ImGuiModuleGLFW::CharEvent(GLFWwindow *window, Codepoint value) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(value);

        return io.WantCaptureKeyboard;
    }

    bool ImGuiModuleGLFW::MouseMotion(GLFWwindow *window, double xpos, double ypos) {
        bool mouse_captured = false;

        for(const auto& context : contexts) {
            context->Bind();

            ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

            if(ImGui::GetIO().WantCaptureMouse) mouse_captured = true;
        }

        return mouse_captured;
    }

    void ImGuiModuleGLFW::CursorEnter(GLFWwindow *window, int entered) {
        ImGui_ImplGlfw_CursorEnterCallback(window, entered);
    }

    bool ImGuiModuleGLFW::MouseButton(GLFWwindow *window, int button, int action, int mods) {
        bool mouse_captured = false;

        for(const auto& context : contexts) {
            context->Bind();

            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

            if(ImGui::GetIO().WantCaptureMouse) mouse_captured = true;
        }

        return mouse_captured;
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

    void ImGuiModuleGLFW::NewFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGuiModule::NewFrame();

        //ImGui::NewFrame();

    }

    void ImGuiModuleGLFW::RenderFrame() {
        ImGuiModule::RenderFrame();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

} // Core