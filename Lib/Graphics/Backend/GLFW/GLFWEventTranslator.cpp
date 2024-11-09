//
// Created by joao on 27/09/23.
//

#include "GLFWEventTranslator.h"

#include "Utils/ReferenceIterator.h"

#include "glfw.h"

// Don't touch:
#define STOP_ON_FIRST_RESPONDER true
#define TO_ALL_RESPONDERS false

#define PropagateEvents(stopOnFirstResponder) {                                          \
    if(!(stopOnFirstResponder)){                                                \
        return std::any_of(guiListeners.begin(), guiListeners.end(), funky);    \
    } else {                                                                    \
        std::for_each(guiListeners.begin(), guiListeners.end(), funky);         \
        return false;                                                           \
    }}


namespace Slab::Graphics {

    GLFWEventTranslator::GLFWEventTranslator() : EventTranslator() {}

    bool GLFWEventTranslator::KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
        // action: GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
        // const int _scancode = glfwGetKeyScancode(GLFW_KEY_X); // scancodes table change with platform

        auto mappedKey = static_cast<KeyMap>(key);
        auto state = static_cast<KeyState>(action);
        ModKeys modKeys{
                (mods & GLFW_MOD_SHIFT) != 0,
                (mods & GLFW_MOD_CONTROL) != 0,
                (mods & GLFW_MOD_ALT) != 0,
                (mods & GLFW_MOD_SUPER) != 0,
                (mods & GLFW_MOD_CAPS_LOCK) != 0,
                (mods & GLFW_MOD_NUM_LOCK) != 0,
        };

        return IterateReferences(syswin_listeners, Func(notifyKeyboard, mappedKey, state, modKeys), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::MouseMotion(GLFWwindow *window, double xpos, double ypos) {
        static auto last_x=0, last_y=0;

        fix dx=xpos-last_x;
        fix dy=ypos-last_y;

        last_x = xpos;
        last_y = ypos;

        return IterateReferences(syswin_listeners, Func(notifyMouseMotion, xpos, ypos, dx, dy), StopOnFirstResponder);
    }

    void GLFWEventTranslator::CursorEnter(GLFWwindow *window, int entered) {
        GLFWListener::CursorEnter(window, entered);
    }

    bool GLFWEventTranslator::MouseButton(GLFWwindow *window, int button, int action, int mods) {
        auto mappedButton = static_cast<enum MouseButton>(button);
        auto state = static_cast<KeyState>(action);
        ModKeys modKeys{
                (mods & GLFW_MOD_SHIFT) != 0,
                (mods & GLFW_MOD_CONTROL) != 0,
                (mods & GLFW_MOD_ALT) != 0,
                (mods & GLFW_MOD_SUPER) != 0,
                (mods & GLFW_MOD_CAPS_LOCK) != 0,
                (mods & GLFW_MOD_NUM_LOCK) != 0,
        };

        if(state == Release)
            return IterateReferences(syswin_listeners, Func(notifyMouseButton, mappedButton, state, modKeys), IterateAll);
        else
            return IterateReferences(syswin_listeners, Func(notifyMouseButton, mappedButton, state, modKeys), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
        return IterateReferences(syswin_listeners, Func(notifyMouseWheel, xoffset, yoffset), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::DroppedFiles(GLFWwindow *window, int count, const char **paths) {
        StrVector pathsVec;

        int i;
        for (i = 0;  i < count;  i++)
            pathsVec.emplace_back(paths[i]);

        auto funky = [&](Pointer<SystemWindowEventListener>& listener){
            return listener->
            notifyFilesDropped(pathsVec);
        };

        return IterateReferences(syswin_listeners, Func(notifyFilesDropped, pathsVec), StopOnFirstResponder);
    }

    void GLFWEventTranslator::Render(GLFWwindow *window) {
        IterateReferences(syswin_listeners, Func(notifyRender));
    }

    void GLFWEventTranslator::ScreenReshape(GLFWwindow *window, int width, int height) {
        IterateReferences(syswin_listeners, Func(notifySystemWindowReshape, width, height));
    }


} // Core