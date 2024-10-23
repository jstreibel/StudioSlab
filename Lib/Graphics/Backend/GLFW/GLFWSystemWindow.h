//
// Created by joao on 10/22/24.
//

#ifndef STUDIOSLAB_GLFWSYSTEMWINDOW_H
#define STUDIOSLAB_GLFWSYSTEMWINDOW_H

#include "Graphics/Backend/SystemWindow.h"
#include "glfw.h"
#include "Utils/Pointer.h"

namespace Slab::Graphics {

    class GLFWSystemWindow : public SystemWindow {
        GLFWwindow* system_window;
        Pointer<GLFWwindow> win;
    public:
        GLFWSystemWindow(GLFWwindow*);

        Int getHeight() const override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_GLFWSYSTEMWINDOW_H
