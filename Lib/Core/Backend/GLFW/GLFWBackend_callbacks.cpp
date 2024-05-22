//
// Created by joao on 21/09/23.
//


#include <GLFW/glfw3.h>

#include "GLFWBackend.h"
#include "Core/Tools/Log.h"

void GLFWBackend::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto &me = GetInstance();

    for(auto &listener : me.listeners) if(listener->KeyboardEvent(window, key, scancode, action, mods)) break;
}

void GLFWBackend::window_char_callback(GLFWwindow *window, unsigned int value) {
    auto &me = GetInstance();

    for(auto &listener : me.listeners) if(listener->CharEvent(window, value)) break;
}

void GLFWBackend::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto &me = GetInstance();

    static int xOld=0, yOld=0;

    int dx = (int)xpos - xOld;
    int dy = (int)ypos - yOld;
    xOld = (int)xpos;
    yOld = (int)ypos;

    me.mouseState.x = (int)xpos;
    me.mouseState.y = (int)ypos;
    me.mouseState.dx = dx;
    me.mouseState.dy = dy;

    for(auto &listener : me.listeners) if(listener->MouseMotion(window, xpos, ypos)) break;
}

void GLFWBackend::cursor_enter_callback(GLFWwindow* window, int entered)
{
    auto &me = GetInstance();

    for(auto &listener : me.listeners) listener->CursorEnter(window, entered);
}

void GLFWBackend::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto &me = GetInstance();

    if(button == GLFW_MOUSE_BUTTON_LEFT)   me.mouseState.leftPressed   = (action == GLFW_PRESS);
    if(button == GLFW_MOUSE_BUTTON_MIDDLE) me.mouseState.centerPressed = (action == GLFW_PRESS);
    if(button == GLFW_MOUSE_BUTTON_RIGHT)  me.mouseState.rightPressed  = (action == GLFW_PRESS);

    for(auto &listener : me.listeners)
        if(listener->MouseButton(window, button, action, mods)) break;
}

void GLFWBackend::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto &me = GetInstance();

    me.mouseState.wheel_dx = xoffset;
    me.mouseState.wheel_dy = yoffset;

    for(auto &listener : me.listeners)
        if(listener->MouseWheel(window, xoffset, yoffset)) break;
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

    for(auto &listener : me.listeners) listener->DroppedFiles(window, count, paths);
}

void GLFWBackend::window_size_callback(GLFWwindow* window, int width, int height) {
    auto &me = GetInstance();

    for(auto &listener : me.listeners) listener->ScreenReshape(window, width, height);
}

auto GLFWBackend::getGLFWWindow() -> GLFWwindow & {
    return *systemWindow;
}

void GLFWBackend::addGLFWListener(Core::GLFWListener *glfwListener, bool highPriority) {
    if(highPriority) listeners.emplace_front(glfwListener);
    else             listeners.emplace_back(glfwListener);
}

bool GLFWBackend::addEventListener(const Core::GUIEventListener_ptr &listener) {
    int w, h;
    glfwGetWindowSize(systemWindow, &w, &h);
    listener->notifyScreenReshape(w, h);

    return GraphicBackend::addEventListener(listener);
}



