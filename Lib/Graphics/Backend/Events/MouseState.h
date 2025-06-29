//
// Created by joao on 04/07/23.
//

#ifndef STUDIOSLAB_MOUSESTATE_H
#define STUDIOSLAB_MOUSESTATE_H

#include "KeyMap.h"
#include "SystemWindowEventListener.h"

namespace Slab::Graphics {

    enum MouseCursor {
        Mouse_ArrowCursor,
        Mouse_IBeamCursor,
        Mouse_CrossHairCursor,
        Mouse_HandCursor,
        Mouse_HResizeCursor,
        Mouse_VResizeCursor,
    };

    class FMouseState : public FSystemWindowEventListener {
    public:
        explicit FMouseState(FOwnerSystemWindow);

        int x = 0,
            y = 0;

        int dx = 0,
            dy = 0;

        int wheel_dx = 0, wheel_dy = 0;

        bool leftPressed = false,
             centerPressed = false,
             rightPressed = false;

        ModKeys mod_keys;

        Timer since_left_pressed;
        Timer since_center_pressed;
        Timer since_right_pressed;

        bool NotifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;

        bool NotifyMouseWheel(double dx, double dy) override;
    };
}

#endif //STUDIOSLAB_MOUSESTATE_H
