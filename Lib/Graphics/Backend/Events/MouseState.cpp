//
// Created by joao on 11/8/24.
//

#include "MouseState.h"

namespace Slab::Graphics {

    bool FMouseState::NotifyMouseButton(EMouseButton Which, EKeyState State, EModKeys ModKeys) {
        this->mod_keys = ModKeys;

        switch (Which) {
            case MouseButton_LEFT:
                Left = State;
                SinceLeftPressed.reset();
                break;
            case MouseButton_MIDDLE:
                Center = State;
                SinceCenterPressed.reset();
                break;
            case MouseButton_RIGHT:
                Right = State;
                SinceRightPressed.reset();
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

    bool FMouseState::NotifyMouseMotion(int _x, int _y, int _dx, int _dy) {
        x = _x;
        y = _y;
        dx = _dx;
        dy = _dy;

        return false;
    }

    bool FMouseState::NotifyMouseWheel(double _dx, double _dy) {
        wheel_dx = (int)_dx;
        wheel_dy = (int)_dy;

        return false;
    }

}