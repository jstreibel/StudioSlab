//
// Created by joao on 6/28/25.
//

#include "WindowManager.h"

namespace Slab {
namespace Graphics {

    void FWindowManager::AddSlabWindow(const Pointer<FSlabWindow>& slab_window)
    {
        this->AddSlabWindow(slab_window, false);
    }
} // Graphics
} // Slab