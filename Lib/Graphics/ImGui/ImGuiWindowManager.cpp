//
// Created by joao on 6/28/25.
//

#include "ImGuiWindowManager.h"

namespace Slab::Graphics {

    FImGuiWindowManager::FImGuiWindowManager(SystemWindow* Parent, Pointer<SlabImGuiContext> Context)
    : FWindowManager(Parent)
    , Context(Context)
    {
    }

    void FImGuiWindowManager::AddSlabWindow(const Pointer<FSlabWindow>&, bool hidden)
    {

    }
} // Slab::Graphics