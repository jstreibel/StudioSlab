//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULEGLFW_H
#define STUDIOSLAB_IMGUIMODULEGLFW_H

#include "ImGuiModule.h"
#include "Core/Backend/GLFW/GLFWListener.h"

namespace Slab::Core {

    class ImGuiModuleGLFW : public ImGuiModule, public GLFWListener {
    public:
        explicit ImGuiModuleGLFW();
        ~ImGuiModuleGLFW() override;

        void beginRender() override;

        void endRender() override;

        bool KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) override;

        bool CharEvent(GLFWwindow *window, Codepoint value) override;

        bool MouseMotion(GLFWwindow *window, double xpos, double ypos) override;

        void CursorEnter(GLFWwindow *window, int entered) override;

        bool MouseButton(GLFWwindow *window, int button, int action, int mods) override;

        bool MouseWheel(GLFWwindow *window, double xoffset, double yoffset) override;

        bool DroppedFiles(GLFWwindow *window, int count, const char **paths) override;

        void ScreenReshape(GLFWwindow *window, int width, int height) override;

    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULEGLFW_H
