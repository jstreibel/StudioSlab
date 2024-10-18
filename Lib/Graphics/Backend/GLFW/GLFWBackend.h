//
// Created by joao on 21/09/23.
//

#ifndef STUDIOSLAB_GLFWBACKEND_H
#define STUDIOSLAB_GLFWBACKEND_H

#include "glfw.h"
#include <list>

#include "Graphics/Types2D.h"

#include "Graphics/Backend/GraphicBackend.h"
#include "GLFWListener.h"
#include "GLFWEventTranslator.h"
#include "Utils/List.h"


namespace Slab::Core {


    class GLFWBackend : public GraphicBackend {
        GLFWEventTranslator glfwEventTranslator;
        List<Volatile<GLFWListener>> listeners{};

        MouseState mouseState;

        GLFWwindow *systemWindow = nullptr;

        void mainLoop();

        static GLFWwindow *newGLFWWindow();

        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void window_char_callback(GLFWwindow *window, unsigned int value);

        static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

        static void cursor_enter_callback(GLFWwindow *window, int entered);

        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

        static void drop_callback(GLFWwindow *window, int count, const char **paths);

        static void window_size_callback(GLFWwindow *window, int width, int height);

        static GLFWBackend &GetInstance();

    public:
        explicit GLFWBackend();

        ~GLFWBackend() override;

        auto getGLFWWindow() -> GLFWwindow &;

        void addGLFWListener(Volatile<Core::GLFWListener> glfwListener, bool highPriority = false);

        static bool GetKeyState(GLFWwindow *window, int key);

        static Graphics::Point2D GetCursorPosition(GLFWwindow *window);

        static bool IsWindowHovered(GLFWwindow *window);

        static bool GetMouseButtonState(GLFWwindow *window, int button);

        auto addEventListener(const Volatile<GUIEventListener> &listener) -> bool override;

        auto getMouseState() const -> MouseState override;

        void setMouseCursor(MouseCursor cursor) override;

        void setSystemWindowTitle(Str title, int handle) override;

        void run() override;

        void terminate() override;

        auto getScreenHeight() const -> Real override;
    };


}

#endif //STUDIOSLAB_GLFWBACKEND_H
