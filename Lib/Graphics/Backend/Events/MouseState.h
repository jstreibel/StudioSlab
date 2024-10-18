//
// Created by joao on 04/07/23.
//

#ifndef STUDIOSLAB_MOUSESTATE_H
#define STUDIOSLAB_MOUSESTATE_H

namespace Slab::Graphics {

    enum MouseCursor {
        Mouse_ArrowCursor,
        Mouse_IBeamCursor,
        Mouse_CrossHairCursor,
        Mouse_HandCursor,
        Mouse_HResizeCursor,
        Mouse_VResizeCursor,
    };

    struct MouseState {
        int x = 0,
                y = 0;

        int dx = 0,
                dy = 0;

        int wheel_dx = 0, wheel_dy = 0;

        bool leftPressed = false,
                centerPressed = false,
                rightPressed = false;
    };
}

#endif //STUDIOSLAB_MOUSESTATE_H
