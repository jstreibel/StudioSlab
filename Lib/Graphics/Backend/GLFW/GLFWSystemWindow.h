//
// Created by joao on 10/22/24.
//

#ifndef STUDIOSLAB_GLFWSYSTEMWINDOW_H
#define STUDIOSLAB_GLFWSYSTEMWINDOW_H

#include "Graphics/Backend/SystemWindow.h"
#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"

#include "glfw.h"
#include "Utils/Pointer.h"
#include "GLFWListener.h"
#include "Utils/List.h"
#include "GLFWEventTranslator.h"

namespace Slab::Graphics {

    class GLFWSystemWindow : public SystemWindow {
        List<Volatile<GLFWListener>> glfw_listeners{};

        GLFWcursor* arrowCursor;
        GLFWcursor* IBeamCursor;
        GLFWcursor* crossHairCursor;
        GLFWcursor* handCursor;
        GLFWcursor* hResizeCursor;
        GLFWcursor* vResizeCursor;

    public:
        explicit GLFWSystemWindow();
        ~GLFWSystemWindow();

        void Render() override;

        Int getWidth() const override;

        Int getHeight() const override;

        void addGLFWListener(const Volatile<Graphics::GLFWListener>& glfwListener, bool highPriority = false);

        void setMouseCursor(MouseCursor cursor) override;

        static void window_focus_callback(GLFWwindow *window, int focused);
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void window_char_callback(GLFWwindow *window, unsigned int value);
        static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
        static void cursor_enter_callback(GLFWwindow *window, int entered);
        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
        static void drop_callback(GLFWwindow *window, int count, const char **paths);
        static void window_size_callback(GLFWwindow *window, int width, int height);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_GLFWSYSTEMWINDOW_H
