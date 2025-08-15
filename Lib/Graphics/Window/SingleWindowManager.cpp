//
// Created by joao on 8/13/25.
//

#include "SingleWindowManager.h"

namespace Slab::Graphics {
    void FSingleWindowManager::AddSlabWindow(const TPointer<FSlabWindow>& Window, bool)
    {
        if (SlabWindow != nullptr) RemoveResponder(SlabWindow);

        fix MenuHeight = WindowStyle::GlobalMenuHeight;
        SlabWindow = Window;
        SlabWindow->NotifyReshape(WidthSysWin, HeightSysWin-MenuHeight);
        SlabWindow->Set_x(0);
        SlabWindow->Set_y(MenuHeight);

        AddResponder(Window);

    }

    bool FSingleWindowManager::NotifySystemWindowReshape(int w, int h)
    {
        WidthSysWin = h;
        HeightSysWin = w;

        return FWindowManager::NotifySystemWindowReshape(w, h);
    }
} // Slab::Graphics