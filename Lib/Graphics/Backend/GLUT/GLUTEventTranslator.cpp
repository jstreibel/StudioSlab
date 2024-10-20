//
// Created by joao on 27/09/23.
//

#include "GLUTEventTranslator.h"
#include "Utils/ReferenceIterator.h"
#include <GL/freeglut.h>

#include <map>

namespace Slab::Graphics {

    KeyMap translateFromGLUTSpecial(unsigned key){
        switch (key) {
            case GLUT_KEY_NUM_LOCK:  return Graphics::Key_NUM_LOCK;
            case GLUT_KEY_BEGIN:     return Graphics::Key_HOME;
            case GLUT_KEY_DELETE:    return Graphics::Key_DELETE;
            case GLUT_KEY_SHIFT_L:   return Graphics::Key_LEFT_SHIFT;
            case GLUT_KEY_SHIFT_R:   return Graphics::Key_RIGHT_SHIFT;
            case GLUT_KEY_CTRL_L:    return Graphics::Key_LEFT_CONTROL;
            case GLUT_KEY_CTRL_R:    return Graphics::Key_RIGHT_CONTROL;
            case GLUT_KEY_ALT_L:     return Graphics::Key_LEFT_ALT;
            case GLUT_KEY_ALT_R:     return Graphics::Key_RIGHT_ALT;
            case GLUT_KEY_SUPER_L:   return Graphics::Key_LEFT_SUPER;
            case GLUT_KEY_SUPER_R:   return Graphics::Key_RIGHT_SUPER;
            case GLUT_KEY_F1:        return Graphics::Key_F1;
            case GLUT_KEY_F2:        return Graphics::Key_F2;
            case GLUT_KEY_F3:        return Graphics::Key_F3;
            case GLUT_KEY_F4:        return Graphics::Key_F4;
            case GLUT_KEY_F5:        return Graphics::Key_F5;
            case GLUT_KEY_F6:        return Graphics::Key_F6;
            case GLUT_KEY_F7:        return Graphics::Key_F7;
            case GLUT_KEY_F8:        return Graphics::Key_F8;
            case GLUT_KEY_F9:        return Graphics::Key_F9;
            case GLUT_KEY_F10:       return Graphics::Key_F10;
            case GLUT_KEY_F11:       return Graphics::Key_F11;
            case GLUT_KEY_F12:       return Graphics::Key_F12;
            case GLUT_KEY_LEFT:      return Graphics::Key_LEFT;
            case GLUT_KEY_UP:        return Graphics::Key_UP;
            case GLUT_KEY_RIGHT:     return Graphics::Key_RIGHT;
            case GLUT_KEY_DOWN:      return Graphics::Key_PAGE_DOWN;
            case GLUT_KEY_PAGE_UP:   return Graphics::Key_PAGE_UP;
            case GLUT_KEY_PAGE_DOWN: return Graphics::Key_PAGE_DOWN;
            case GLUT_KEY_HOME:      return Graphics::Key_HOME;
            case GLUT_KEY_END:       return Graphics::Key_END;
            case GLUT_KEY_INSERT:    return Graphics::Key_INSERT;

            default: return Graphics::Key_UNKNOWN;
        }
    }

    bool GLUTEventTranslator::keyboard(unsigned char key, int x, int y) {
        auto key_cast = static_cast<Graphics::KeyMap>(key);
        auto action = Graphics::Press;
        auto mod_keys = ModKeys{};

        return IterateReferences(guiListeners, Func(notifyKeyboard, key_cast, action, mod_keys));
    }

    bool GLUTEventTranslator::keyboardUp(unsigned char key, int x, int y) {
        auto key_cast = static_cast<Graphics::KeyMap>(key);
        auto action = Graphics::Release;
        auto mod_keys = ModKeys{};

        return IterateReferences(guiListeners, Func(notifyKeyboard, key_cast, action, mod_keys));
    }

    bool GLUTEventTranslator::keyboardSpecial(int key, int x, int y) {
        auto key_cast = translateFromGLUTSpecial(key);
        auto action = Graphics::Press;
        auto mod_keys = ModKeys{};

        return IterateReferences(guiListeners, Func(notifyKeyboard, key_cast, action, mod_keys));
    }

    bool GLUTEventTranslator::keyboardSpecialUp(int key, int x, int y) {
        auto key_cast = translateFromGLUTSpecial(key);
        auto action = Graphics::Release;
        auto mod_keys = ModKeys{};

        return IterateReferences(guiListeners, Func(notifyKeyboard, key_cast, action, mod_keys));
    }

    bool GLUTEventTranslator::mouseButton(int button, int state, int x, int y) {
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;

        if (button == GLUT_LEFT_BUTTON) mouseState.leftPressed = (state == GLUT_DOWN);
        else if (button == GLUT_MIDDLE_BUTTON) mouseState.centerPressed = (state == GLUT_DOWN);
        else if (button == GLUT_RIGHT_BUTTON) mouseState.rightPressed = (state == GLUT_DOWN);

        std::map<int, Graphics::MouseButton> mouseMap = {
                {GLUT_LEFT_BUTTON,   Graphics::MouseButton_LEFT   },
                {GLUT_MIDDLE_BUTTON, Graphics::MouseButton_MIDDLE },
                {GLUT_RIGHT_BUTTON,  Graphics::MouseButton_RIGHT  },
        };

        std::map<int, Graphics::KeyState> buttonStateMap = {
                {GLUT_UP,   Graphics::Release },
                {GLUT_DOWN, Graphics::Press   },
        };

        auto button_map = mouseMap[button];
        auto state_map = buttonStateMap[state];
        auto mod_keys = ModKeys{};

        IterateReferences(guiListeners, Func(notifyMouseButton, button_map, state_map, mod_keys));

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

        return IterateReferences(guiListeners, Func(notifyMouseWheel, dx, dy));
    }

    bool GLUTEventTranslator::mouseMotion(int x, int y) {
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;

        return IterateReferences(guiListeners, Func(notifyMouseMotion, x, y));
    }

    bool GLUTEventTranslator::render() {
        return IterateReferences(guiListeners, Func(notifyRender));
    }

    bool GLUTEventTranslator::idle() { return false; }

    bool GLUTEventTranslator::reshape(int w, int h) {
        return IterateReferences(guiListeners, Func(notifySystemWindowReshape, w, h));
    }

    const MouseState &GLUTEventTranslator::getMouseState() const {
        return mouseState;
    }

} // Backend