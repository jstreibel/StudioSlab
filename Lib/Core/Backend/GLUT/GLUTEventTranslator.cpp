//
// Created by joao on 27/09/23.
//

#include "GLUTEventTranslator.h"
#include <GL/freeglut.h>

#include <map>

namespace Slab::Core {

    KeyMap translateFromGLUTSpecial(unsigned key){
        switch (key) {
            case GLUT_KEY_NUM_LOCK:  return Core::Key_NUM_LOCK;
            case GLUT_KEY_BEGIN:     return Core::Key_HOME;
            case GLUT_KEY_DELETE:    return Core::Key_DELETE;
            case GLUT_KEY_SHIFT_L:   return Core::Key_LEFT_SHIFT;
            case GLUT_KEY_SHIFT_R:   return Core::Key_RIGHT_SHIFT;
            case GLUT_KEY_CTRL_L:    return Core::Key_LEFT_CONTROL;
            case GLUT_KEY_CTRL_R:    return Core::Key_RIGHT_CONTROL;
            case GLUT_KEY_ALT_L:     return Core::Key_LEFT_ALT;
            case GLUT_KEY_ALT_R:     return Core::Key_RIGHT_ALT;
            case GLUT_KEY_SUPER_L:   return Core::Key_LEFT_SUPER;
            case GLUT_KEY_SUPER_R:   return Core::Key_RIGHT_SUPER;
            case GLUT_KEY_F1:        return Core::Key_F1;
            case GLUT_KEY_F2:        return Core::Key_F2;
            case GLUT_KEY_F3:        return Core::Key_F3;
            case GLUT_KEY_F4:        return Core::Key_F4;
            case GLUT_KEY_F5:        return Core::Key_F5;
            case GLUT_KEY_F6:        return Core::Key_F6;
            case GLUT_KEY_F7:        return Core::Key_F7;
            case GLUT_KEY_F8:        return Core::Key_F8;
            case GLUT_KEY_F9:        return Core::Key_F9;
            case GLUT_KEY_F10:       return Core::Key_F10;
            case GLUT_KEY_F11:       return Core::Key_F11;
            case GLUT_KEY_F12:       return Core::Key_F12;
            case GLUT_KEY_LEFT:      return Core::Key_LEFT;
            case GLUT_KEY_UP:        return Core::Key_UP;
            case GLUT_KEY_RIGHT:     return Core::Key_RIGHT;
            case GLUT_KEY_DOWN:      return Core::Key_PAGE_DOWN;
            case GLUT_KEY_PAGE_UP:   return Core::Key_PAGE_UP;
            case GLUT_KEY_PAGE_DOWN: return Core::Key_PAGE_DOWN;
            case GLUT_KEY_HOME:      return Core::Key_HOME;
            case GLUT_KEY_END:       return Core::Key_END;
            case GLUT_KEY_INSERT:    return Core::Key_INSERT;

            default: return Core::Key_UNKNOWN;
        }
    }

    bool GLUTEventTranslator::keyboard(unsigned char key, int x, int y) {
        for(auto &listener : guiListeners)
            if(listener->notifyKeyboard(static_cast<Core::KeyMap>(key), Core::Press, {})) return true;

        return false;
    }

    bool GLUTEventTranslator::keyboardUp(unsigned char key, int x, int y) {
        for(auto &listener : guiListeners)
            if(listener->notifyKeyboard(static_cast<Core::KeyMap>(key), Core::Release, {})) return true;

        return false;
    }

    bool GLUTEventTranslator::keyboardSpecial(int key, int x, int y) {
        auto keyMap = translateFromGLUTSpecial(key);

        for(auto &listener : guiListeners)
             if(listener->notifyKeyboard(keyMap, Core::Press, {})) return true;

        return false;
    }

    bool GLUTEventTranslator::keyboardSpecialUp(int key, int x, int y) {
        auto keyMap = translateFromGLUTSpecial(key);

        for(auto &listener : guiListeners)
            if(listener->notifyKeyboard(keyMap, Core::Release, {})) return true;

        return false;
    }

    bool GLUTEventTranslator::mouseButton(int button, int state, int x, int y) {
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;

        if (button == GLUT_LEFT_BUTTON) mouseState.leftPressed = (state == GLUT_DOWN);
        else if (button == GLUT_MIDDLE_BUTTON) mouseState.centerPressed = (state == GLUT_DOWN);
        else if (button == GLUT_RIGHT_BUTTON) mouseState.rightPressed = (state == GLUT_DOWN);

        std::map<int, Core::MouseButton> mouseMap = {
                {GLUT_LEFT_BUTTON,   Core::MouseButton_LEFT   },
                {GLUT_MIDDLE_BUTTON, Core::MouseButton_MIDDLE },
                {GLUT_RIGHT_BUTTON,  Core::MouseButton_RIGHT  },
        };

        std::map<int, Core::KeyState> buttonStateMap = {
                {GLUT_UP,   Core::Release },
                {GLUT_DOWN, Core::Press   },
        };

        for (auto &listener: guiListeners)
            if(listener->notifyMouseButton(mouseMap[button], buttonStateMap[state], {})) return true;

        return false;
    }

    bool GLUTEventTranslator::mousePassiveMotion(int x, int y) {
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;

        return mouseMotion(x, y);
    }

    bool GLUTEventTranslator::mouseWheel(int wheel, int direction, int x, int y) {
        auto dx = wheel==1 ? direction : 0;
        auto dy = wheel==0 ? direction : 0;

        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;
        mouseState.wheel_dx = dx;
        mouseState.wheel_dy = dy;

        for (auto &listener: guiListeners) {
            if (listener->notifyMouseWheel(dx, dy)) return true;
        }

        return false;
    }

    bool GLUTEventTranslator::mouseMotion(int x, int y) {
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;

        for (auto &listener: guiListeners) if(listener->notifyMouseMotion(x, y)) return true;

        return false;
    }

    bool GLUTEventTranslator::render() { for (auto &listener: guiListeners) listener->notifyRender(); return false; }

    bool GLUTEventTranslator::idle() { return false; }

    bool GLUTEventTranslator::reshape(int w, int h) {
        for (auto &listener: guiListeners) listener->notifyScreenReshape(w, h);

        return false;
    }

    const MouseState &GLUTEventTranslator::getMouseState() const {
        return mouseState;
    }

} // Backend