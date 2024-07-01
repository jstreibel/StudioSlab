//
// Created by joao on 21/09/23.
//


#include <GLFW/glfw3.h>

#include "GLFWBackend.h"
#include "Core/Tools/Log.h"

#include "Utils/ReferenceIterator.h"

namespace Slab::Core {

    void GLFWBackend::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto &me = GetInstance();

        IterateReferences(me.listeners,
                          Func(KeyboardEvent, window, key, scancode, action, mods),
                          StopOnFirstResponder);

        // for (auto &listener: me.listeners) if (listener->KeyboardEvent(window, key, scancode, action, mods)) break;
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

        IterateReferences(me.listeners,
                          Func(MouseButton, window, button, action, mods),
                          IterateAll);
    }

    void GLFWBackend::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        auto &me = GetInstance();

        me.mouseState.wheel_dx = xoffset;
        me.mouseState.wheel_dy = yoffset;

        IterateReferences(me.listeners,
                          Func(MouseWheel, window, xoffset, yoffset),
                          IterateAll);
    }

    void GLFWBackend::drop_callback(GLFWwindow *window, int count, const char **paths) {
        StrVector pathsVec;

        auto &log = Log::Info() << "Dropped files: ";
        int i;
        for (i = 0; i < count; i++) {
            log << "\n\t\t" << paths[i];
            pathsVec.emplace_back(paths[i]);
        }

        log << Log::Flush;

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

    void GLFWBackend::addGLFWListener(Reference<Core::GLFWListener> glfwListener, bool highPriority) {
        if (highPriority) listeners.emplace_front(glfwListener);
        else listeners.emplace_back(glfwListener);
    }

    bool GLFWBackend::addEventListener(const Reference<Core::GUIEventListener> &listener) {
        int w, h;
        glfwGetWindowSize(systemWindow, &w, &h);

        listener.lock()->notifyScreenReshape(w, h);

        return GraphicBackend::addEventListener(listener);
    }


}