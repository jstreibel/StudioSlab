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

    GLFWEventTranslator::GLFWEventTranslator() : FEventTranslator() {}

    bool GLFWEventTranslator::KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
        // action: GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
        // const int _scancode = glfwGetKeyScancode(GLFW_KEY_X); // scancodes table change with platform

        auto mappedKey = static_cast<EKeyMap>(key);
        auto state = static_cast<EKeyState>(action);
        EModKeys modKeys{
                (mods & GLFW_MOD_SHIFT)     != 0 ? Press : Release,
                (mods & GLFW_MOD_CONTROL)   != 0 ? Press : Release,
                (mods & GLFW_MOD_ALT)       != 0 ? Press : Release,
                (mods & GLFW_MOD_SUPER)     != 0 ? Press : Release,
                (mods & GLFW_MOD_CAPS_LOCK) != 0 ? Press : Release,
                (mods & GLFW_MOD_NUM_LOCK)  != 0 ? Press : Release,
        };

        return IterateReferences(SysWinListeners, Func(NotifyKeyboard, mappedKey, state, modKeys), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::CharEvent(GLFWwindow *, UInt codepoint) {
        return IterateReferences(SysWinListeners, Func(NotifyCharacter, codepoint), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::MouseMotion(GLFWwindow *window, double xpos, double ypos) {
        static auto last_x=0, last_y=0;

        fix dx=xpos-last_x;
        fix dy=ypos-last_y;

        last_x = xpos;
        last_y = ypos;

        return IterateReferences(SysWinListeners, Func(NotifyMouseMotion, xpos, ypos, dx, dy), StopOnFirstResponder);
    }

    void GLFWEventTranslator::CursorEnter(GLFWwindow *window, int entered) {
        GLFWListener::CursorEnter(window, entered);
    }

    bool GLFWEventTranslator::MouseButton(GLFWwindow *window, int button, int action, int mods) {
        auto mappedButton = static_cast<enum EMouseButton>(button);
        auto state = static_cast<EKeyState>(action);
        EModKeys modKeys{
                (mods & GLFW_MOD_SHIFT)     != 0 ? Press : Release,
                (mods & GLFW_MOD_CONTROL)   != 0 ? Press : Release,
                (mods & GLFW_MOD_ALT)       != 0 ? Press : Release,
                (mods & GLFW_MOD_SUPER)     != 0 ? Press : Release,
                (mods & GLFW_MOD_CAPS_LOCK) != 0 ? Press : Release,
                (mods & GLFW_MOD_NUM_LOCK)  != 0 ? Press : Release,
        };

        if(state == Release)
            return IterateReferences(SysWinListeners, Func(NotifyMouseButton, mappedButton, state, modKeys), IterateAll);
        else
            return IterateReferences(SysWinListeners, Func(NotifyMouseButton, mappedButton, state, modKeys), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
        return IterateReferences(SysWinListeners, Func(NotifyMouseWheel, xoffset, yoffset), StopOnFirstResponder);
    }

    bool GLFWEventTranslator::DroppedFiles(GLFWwindow *window, int count, const char **paths) {
        StrVector pathsVec;

        int i;
        for (i = 0;  i < count;  i++)
            pathsVec.emplace_back(paths[i]);

        auto funky = [&](Pointer<FSystemWindowEventListener>& listener){
            return listener->
            NotifyFilesDropped(pathsVec);
        };

        return IterateReferences(SysWinListeners, Func(NotifyFilesDropped, pathsVec), StopOnFirstResponder);
    }

    void GLFWEventTranslator::Render(GLFWwindow *window) {
        IterateReferences(SysWinListeners, Func(NotifyRender));
    }

    void GLFWEventTranslator::ScreenReshape(GLFWwindow *window, int width, int height) {
        IterateReferences(SysWinListeners, Func(NotifySystemWindowReshape, width, height));
    }


} // Core