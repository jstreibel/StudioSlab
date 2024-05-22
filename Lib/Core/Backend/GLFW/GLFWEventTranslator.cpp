//
// Created by joao on 27/09/23.
//

#include "GLFWEventTranslator.h"

#define STOP_ON_FIRST_RESPONDER true

#define PropagateEvents \
    if(!STOP_ON_FIRST_RESPONDER){                                               \
        return std::any_of(guiListeners.begin(), guiListeners.end(), funky);    \
    } else {                                                                    \
        std::for_each(guiListeners.begin(), guiListeners.end(), funky);         \
        return false;                                                           \
    }


namespace Core {

    bool GLFWEventTranslator::KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
        // action: GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
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

        auto funky = [&mappedKey, &state, &modKeys](GUIEventListener_ptr& listener){
            return listener->
                    notifyKeyboard(mappedKey, state, modKeys);
        };

        PropagateEvents;
    }

    bool GLFWEventTranslator::MouseMotion(GLFWwindow *window, double xpos, double ypos) {

        auto funky = [&](GUIEventListener_ptr& listener){
            return listener->
            notifyMouseMotion((int)xpos, (int)ypos);
        };

        PropagateEvents;
    }

    void GLFWEventTranslator::CursorEnter(GLFWwindow *window, int entered) {
        GLFWListener::CursorEnter(window, entered);
    }

    bool GLFWEventTranslator::MouseButton(GLFWwindow *window, int button, int action, int mods) {
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

        auto funky = [&](GUIEventListener_ptr& listener){
            return listener->
            notifyMouseButton(mappedButton, state, modKeys);
        };

        PropagateEvents;
    }

    bool GLFWEventTranslator::MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
        auto funky = [&](GUIEventListener_ptr& listener){
            return listener->
            notifyMouseWheel(xoffset, yoffset);
        };

        PropagateEvents;
    }

    bool GLFWEventTranslator::DroppedFiles(GLFWwindow *window, int count, const char **paths) {
        StrVector pathsVec;

        int i;
        for (i = 0;  i < count;  i++)
            pathsVec.emplace_back(paths[i]);

        auto funky = [&](GUIEventListener_ptr& listener){
            return listener->
            notifyFilesDropped(pathsVec);
        };

        PropagateEvents;
    }

    void GLFWEventTranslator::Render(GLFWwindow *window) {
        auto funky = [&](GUIEventListener_ptr& listener){
            return listener->
            notifyRender();
        };

        std::for_each(guiListeners.begin(), guiListeners.end(), funky);
    }

    void GLFWEventTranslator::ScreenReshape(GLFWwindow *window, int width, int height) {
        auto funky = [&](GUIEventListener_ptr& listener){
            return listener->
            notifyScreenReshape(width, height);
        };

        std::for_each(guiListeners.begin(), guiListeners.end(), funky);
    }


} // Core