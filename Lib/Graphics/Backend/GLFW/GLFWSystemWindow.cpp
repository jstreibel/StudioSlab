//
// Created by joao on 10/22/24.
//

#include "GLFWSystemWindow.h"

namespace Slab {
    namespace Graphics {
        GLFWSystemWindow::GLFWSystemWindow(GLFWwindow* win) : system_window(win) {

        }

        int GLFWSystemWindow::getHeight() const {
            int w, h;
            glfwGetWindowSize(system_window, &w, &h);

            return h;
        }


    } // Slab
} // Graphics