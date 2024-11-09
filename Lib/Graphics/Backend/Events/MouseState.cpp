//
// Created by joao on 11/8/24.
//

#include "MouseState.h"

namespace Slab::Graphics {

    bool MouseState::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        mod_keys = keys;

        switch (button) {
            case MouseButton_LEFT:
                leftPressed = state==Press;
                since_left_pressed.reset();
                break;
            case MouseButton_MIDDLE:
                centerPressed = state==Press;
                since_center_pressed.reset();
                break;
            case MouseButton_RIGHT:
                rightPressed = state==Press;
                since_right_pressed.reset();
                break;
            case MouseButton_4:
                break;
            case MouseButton_5:
                break;
            case MouseButton_6:
                break;
            case MouseButton_7:
                break;
            case MouseButton_8:
                break;
        }

        return false;
    }

    bool MouseState::notifyMouseMotion(int _x, int _y, int _dx, int _dy) {
        x = _x;
        y = _y;
        dx = _dx;
        dy = _dy;

        return false;
    }

    bool MouseState::notifyMouseWheel(double _dx, double _dy) {
        wheel_dx = _dx;
        wheel_dy = _dy;

        return false;
    }
}