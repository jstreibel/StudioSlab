//
// Created by joao on 27/09/23.
//

#include "GLFWListener.h"

namespace Slab::Graphics {

    bool GLFWListener::KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
        return false;
    }

    bool GLFWListener::CharEvent(GLFWwindow *window, Codepoint value) {
        return false;
    }

    bool GLFWListener::MouseMotion(GLFWwindow *window, double xpos, double ypos) {
        return false;
    }

    void GLFWListener::CursorEnter(GLFWwindow *window, int entered) {

    }

    bool GLFWListener::MouseButton(GLFWwindow *window, int button, int action, int mods) {
        return false;
    }

    bool GLFWListener::MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
        return false;
    }

    bool GLFWListener::DroppedFiles(GLFWwindow *window, int count, const char **paths) {
        return false;
    }

    void GLFWListener::Render(GLFWwindow *window) {

    }

    void GLFWListener::ScreenReshape(GLFWwindow *window, int width, int height) {

    }

} // Core