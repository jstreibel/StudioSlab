//
// Created by joao on 6/28/25.
//

#include "WindowManager.h"

namespace Slab::Graphics {

    void FWindowManager::AddSlabWindow(const TPointer<FSlabWindow>& slab_window)
    {
        this->AddSlabWindow(slab_window, false);
    }
} // Slab::Graphics