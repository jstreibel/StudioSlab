//
// Created by joao on 04/07/23.
//

#ifndef STUDIOSLAB_MOUSESTATE_H
#define STUDIOSLAB_MOUSESTATE_H

#include "KeyMap.h"
#include "SystemWindowEventListener.h"

namespace Slab::Graphics {

    enum FMouseCursor {
        Mouse_ArrowCursor,
        Mouse_IBeamCursor,
        Mouse_CrossHairCursor,
        Mouse_HandCursor,
        Mouse_HResizeCursor,
        Mouse_VResizeCursor,
    };

    class FMouseState final : public FPlatformWindowEventListener {
    public:
        int x = 0,
            y = 0;

        int dx = 0,
            dy = 0;

        int wheel_dx = 0, wheel_dy = 0;

        EKeyState Left   = Release;
        EKeyState Center = Release;
        EKeyState Right  = Release;

        bool IsLeftPressed() const { return Left != Release; }
        bool IsCenterPressed() const { return Center != Release; }
        bool IsRightPressed() const { return Right != Release; }

        EModKeys mod_keys;

        Timer SinceLeftPressed;
        Timer SinceCenterPressed;
        Timer SinceRightPressed;

        bool NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys keys) override;

        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;

        bool NotifyMouseWheel(double dx, double dy) override;
    };
}

#endif //STUDIOSLAB_MOUSESTATE_H
