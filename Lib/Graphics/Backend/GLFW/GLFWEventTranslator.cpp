//
// Created by joao on 27/09/23.
//

#include "GLFWEventTranslator.h"

#include "Utils/ReferenceIterator.h"

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

        return IterateReferences(guiListeners, Func(notifyKeyboard, mappedKey, state, modKeys), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::MouseMotion(GLFWwindow *window, double xpos, double ypos) {
        return IterateReferences(guiListeners, Func(notifyMouseMotion, xpos, ypos), StopOnFirstResponder);
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
            return IterateReferences(guiListeners, Func(notifyMouseButton, mappedButton, state, modKeys), IterateAll);
        else
            return IterateReferences(guiListeners, Func(notifyMouseButton, mappedButton, state, modKeys), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
        return IterateReferences(guiListeners, Func(notifyMouseWheel, xoffset, yoffset), StopOnFirstResponder);
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

        return IterateReferences(guiListeners, Func(notifyFilesDropped, pathsVec), StopOnFirstResponder);
    }

    void GLFWEventTranslator::Render(GLFWwindow *window) {
        IterateReferences(guiListeners, Func(notifyRender));
    }

    void GLFWEventTranslator::ScreenReshape(GLFWwindow *window, int width, int height) {
        IterateReferences(guiListeners, Func(notifyScreenReshape, width, height));
    }


} // Core