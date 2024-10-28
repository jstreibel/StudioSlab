//
// Created by joao on 21/09/23.
//

#include "glfw.h"

#include "GLFWBackend.h"
#include "Core/Tools/Log.h"

#include "Utils/ReferenceIterator.h"

namespace Slab::Graphics {

    struct WindowedModeConfig {
        int x=10, y=10;
        int width=3400, height=1700;
    } windowed_mode_config;

    bool isWindowFullscreen(GLFWwindow* window) {
        return glfwGetWindowMonitor(window) != nullptr;

    }

    void setWindowFullscreen(GLFWwindow* window) {
        // Get the primary monitor
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if (!primaryMonitor) {
            // Handle error: no monitor found
            return;
        }

        // Get the video mode of the monitor
        const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
        if (!videoMode) {
            // Handle error: no video mode found
            return;
        }

        // Store current windowed mode size and position
        glfwGetWindowPos(window, &windowed_mode_config.x, &windowed_mode_config.y);
        glfwGetWindowSize(window, &windowed_mode_config.width, &windowed_mode_config.height);

        // Set the window to fullscreen
        glfwSetWindowMonitor(window, primaryMonitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
    }

    void setWindowWindowed(GLFWwindow* window) {
        // Set the window back to windowed mode
        glfwSetWindowMonitor(window, nullptr,
                             windowed_mode_config.x, windowed_mode_config.y,
                             windowed_mode_config.width, windowed_mode_config.height, 0);
    }

    void toggle_window_fullscreen(GLFWwindow *window) {
        if (isWindowFullscreen(window)) {
            setWindowWindowed(window);
        } else {
            setWindowFullscreen(window);
        }
    }

    void GLFWBackend::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto &me = GetInstance();

        if(key == GLFW_KEY_F11 && mods==GLFW_MOD_SHIFT && action == GLFW_PRESS)
            toggle_window_fullscreen(window);

        IterateReferences(me.listeners,
                          Func(KeyboardEvent, window, key, scancode, action, mods),
                          StopOnFirstResponder);
    }

    void GLFWBackend::window_char_callback(GLFWwindow *window, unsigned int value) {
        auto &me = GetInstance();

        IterateReferences(me.listeners,
                          Func(CharEvent, window, value),
                          StopOnFirstResponder);
    }

    void GLFWBackend::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
        auto &me = GetInstance();

        static int xOld = 0, yOld = 0;

        int dx = (int) xpos - xOld;
        int dy = (int) ypos - yOld;
        xOld = (int) xpos;
        yOld = (int) ypos;

        me.mouseState.x = (int) xpos;
        me.mouseState.y = (int) ypos;
        me.mouseState.dx = dx;
        me.mouseState.dy = dy;

        IterateReferences(me.listeners,
                          Func(MouseMotion, window, xpos, ypos),
                          StopOnFirstResponder);
    }

    void GLFWBackend::cursor_enter_callback(GLFWwindow *window, int entered) {
        auto &me = GetInstance();

        IterateReferences(me.listeners,
                          FuncRun(CursorEnter, window, entered),
                          IterateAll);
    }

    void GLFWBackend::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        auto &me = GetInstance();

        if (button == GLFW_MOUSE_BUTTON_LEFT) me.mouseState.leftPressed = (action == GLFW_PRESS);
        if (button == GLFW_MOUSE_BUTTON_MIDDLE) me.mouseState.centerPressed = (action == GLFW_PRESS);
        if (button == GLFW_MOUSE_BUTTON_RIGHT) me.mouseState.rightPressed = (action == GLFW_PRESS);

        auto iteration_policy = action==GLFW_RELEASE ? IterateAll : StopOnFirstResponder;

        IterateReferences(me.listeners,
                          Func(MouseButton, window, button, action, mods),
                          iteration_policy);
    }

    void GLFWBackend::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        auto &me = GetInstance();

        me.mouseState.wheel_dx = xoffset;
        me.mouseState.wheel_dy = yoffset;

        IterateReferences(me.listeners,
                          Func(MouseWheel, window, xoffset, yoffset),
                          StopOnFirstResponder);
    }

    void GLFWBackend::drop_callback(GLFWwindow *window, int count, const char **paths) {
        StrVector pathsVec;

        auto &log = Core::Log::Info() << "Dropped files: ";
        int i;
        for (i = 0; i < count; i++) {
            log << "\n\t\t" << paths[i];
            pathsVec.emplace_back(paths[i]);
        }

        log << Core::Log::Flush;

        auto &me = GetInstance();

        IterateReferences(me.listeners, Func(DroppedFiles, window, count, paths), IterateAll);
    }

    void GLFWBackend::window_size_callback(GLFWwindow *window, int width, int height) {
        auto &me = GetInstance();

        IterateReferences(me.listeners, FuncRun(ScreenReshape, window, width, height), IterateAll);
    }

    auto GLFWBackend::getGLFWWindow() -> GLFWwindow & {
        return *systemWindow;
    }

    void GLFWBackend::addGLFWListener(Volatile<GLFWListener> glfwListener, bool highPriority) {
        if (highPriority) listeners.emplace_front(glfwListener);
        else listeners.emplace_back(glfwListener);
    }

    bool GLFWBackend::addEventListener(const Volatile<SystemWindowEventListener> &listener) {
        int w, h;
        glfwGetWindowSize(systemWindow, &w, &h);

        listener.lock()->notifySystemWindowReshape(w, h);

        return GraphicBackend::addEventListener(listener);
    }


}