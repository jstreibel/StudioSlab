//
// Created by joao on 27/09/23.
//

#include "GLFWListener.h"

namespace Slab::Graphics {

    bool FGLFWListener::KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
        return false;
    }

    bool FGLFWListener::CharEvent(GLFWwindow *window, Codepoint value) {
        return false;
    }

    bool FGLFWListener::MouseMotion(GLFWwindow *window, double xpos, double ypos) {
        return false;
    }

    void FGLFWListener::CursorEnter(GLFWwindow *window, int entered) {

    }

    bool FGLFWListener::MouseButton(GLFWwindow *window, int button, int action, int mods) {
        return false;
    }

    bool FGLFWListener::MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
        return false;
    }

    bool FGLFWListener::DroppedFiles(GLFWwindow *window, int count, const char **paths) {
        return false;
    }

    void FGLFWListener::Render(GLFWwindow *window) {

    }

    void FGLFWListener::ScreenReshape(GLFWwindow *window, int width, int height) {

    }

} // Core