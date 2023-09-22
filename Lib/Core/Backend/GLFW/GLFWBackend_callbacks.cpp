//
// Created by joao on 21/09/23.
//


#include <GLFW/glfw3.h>

#include "GLFWBackend.h"
#include "Core/Tools/Log.h"

void GLFWBackend::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{   // action: GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
    // const int _scancode = glfwGetKeyScancode(GLFW_KEY_X); // scancodes table change with platform

    auto mappedKey = static_cast<Core::KeyMap>(key);
    auto state = static_cast<Core::KeyState>(action);
    Core::ModKeys modKeys{
        (mods & GLFW_MOD_SHIFT) != 0,
        (mods & GLFW_MOD_CONTROL) != 0,
        (mods & GLFW_MOD_ALT) != 0,
        (mods & GLFW_MOD_SUPER) != 0,
        (mods & GLFW_MOD_CAPS_LOCK) != 0,
        (mods & GLFW_MOD_NUM_LOCK) != 0,
    };

    auto &me = GetInstance();

    for(auto &listener : me.listeners)
        listener->notifyKeyboard(mappedKey, state, modKeys);
}

void GLFWBackend::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto &me = GetInstance();

    for(auto &listener : me.listeners)
        listener->notifyMouseMotion(xpos, ypos);
}

void GLFWBackend::cursor_enter_callback(GLFWwindow* window, int entered)
{
    if (entered)
    {
        // The cursor entered the content area of the window
    }
    else
    {
        // The cursor left the content area of the window
    }
}

void GLFWBackend::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto mappedButton = static_cast<Core::MouseButton>(button);
    auto state = static_cast<Core::KeyState>(action);
    Core::ModKeys modKeys{
            (mods & GLFW_MOD_SHIFT) != 0,
            (mods & GLFW_MOD_CONTROL) != 0,
            (mods & GLFW_MOD_ALT) != 0,
            (mods & GLFW_MOD_SUPER) != 0,
            (mods & GLFW_MOD_CAPS_LOCK) != 0,
            (mods & GLFW_MOD_NUM_LOCK) != 0,
    };

    auto &me = GetInstance();

    for(auto &listener : me.listeners)
        listener->notifyMouseButton(mappedButton, state, modKeys);
}

void GLFWBackend::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto &me = GetInstance();

    for(auto &listener : me.listeners)
        listener->notifyMouseWheel(xoffset, yoffset);
}

void GLFWBackend::drop_callback(GLFWwindow* window, int count, const char** paths)
{
    StrVector pathsVec;

    auto &log = Log::Info() << "Dropped files: ";
    int i;
    for (i = 0;  i < count;  i++) {
        log << "\n\t\t" << paths[i];
        pathsVec.emplace_back(paths[i]);
    }

    log << Log::Flush;

    auto &me = GetInstance();

    for(auto &listener : me.listeners)
        listener->notifyFilesDropped(pathsVec);
}

void GLFWBackend::window_size_callback(GLFWwindow* window, int width, int height) {
    auto &me = GetInstance();

    for(auto &listener : me.listeners)
        listener->notifyScreenReshape(width, height);
}
