//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_GLFWLISTENER_H
#define STUDIOSLAB_GLFWLISTENER_H

#include <GLFW/glfw3.h>

namespace Core {

    typedef unsigned int Codepoint;

    class GLFWListener {

    public:
        virtual bool KeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

        virtual bool CharEvent(GLFWwindow* window, Codepoint value);

        virtual bool MouseMotion(GLFWwindow* window, double xpos, double ypos);

        virtual void CursorEnter(GLFWwindow* window, int entered);

        virtual bool MouseButton(GLFWwindow* window, int button, int action, int mods);

        virtual bool MouseWheel(GLFWwindow* window, double xoffset, double yoffset);

        virtual bool DroppedFiles(GLFWwindow* window, int count, const char**paths);

        virtual void Render(GLFWwindow *window);

        virtual void ScreenReshape(GLFWwindow* window, int width, int height);

    };

} // Core

#endif //STUDIOSLAB_GLFWLISTENER_H
