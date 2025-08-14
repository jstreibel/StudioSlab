//
// Created by joao on 10/22/24.
//

#ifndef STUDIOSLAB_GLFWSYSTEMWINDOW_H
#define STUDIOSLAB_GLFWSYSTEMWINDOW_H

#include "Graphics/Backend/PlatformWindow.h"
#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"

#include "glfw.h"
#include "Utils/Pointer.h"
#include "GLFWListener.h"
#include "Utils/List.h"
#include "GLFWEventTranslator.h"

namespace Slab::Graphics {

    class FGLFWPlatformWindow : public FPlatformWindow {
        friend class GLFWBackend;

        TList<TVolatile<FGLFWListener>> GLFWListeners{};

        GLFWcursor* arrowCursor;
        GLFWcursor* IBeamCursor;
        GLFWcursor* crossHairCursor;
        GLFWcursor* handCursor;
        GLFWcursor* hResizeCursor;
        GLFWcursor* vResizeCursor;

    public:
        explicit FGLFWPlatformWindow();
        ~FGLFWPlatformWindow() override;

        void Cycle() override;
        void Flush() override;

        Int GetWidth() const override;

        Int GetHeight() const override;

        void SignalClose() override;
        bool ShouldClose() const override;

        void AddGLFWListener(const TVolatile<Graphics::FGLFWListener>& glfwListener, bool HighPriority = false);

        void SetMouseCursor(FMouseCursor Cursor) override;

        void SetSystemWindowTitle(const Str &title) override;


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
