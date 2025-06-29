//
// Created by joao on 6/28/25.
//

#include "ImGuiWindowManager.h"

namespace Slab::Graphics {

    FImGuiWindowManager::FImGuiWindowManager(SystemWindow* Parent, const Pointer<SlabImGuiContext>& Context)
    : FWindowManager(Parent)
    , Context(Context)
    {

    }

    void FImGuiWindowManager::AddSlabWindow(const Pointer<FSlabWindow>& SlabWindow, bool Hidden)
    {
        auto WrapperWindow = New<FImGuiWindow>(SlabWindow, Context);
        Windows.emplace_back(WrapperWindow);
    }

    bool FImGuiWindowManager::NotifyRender()
    {
        // We don't want to call FWindowManager::NotifyRender(); because, while 'Context' is this
        // class's delegate responder, we don't want it to be the delegate responder to NotifyRender

        for (const auto& WrapperWindow : Windows)
        {
            WrapperWindow->NotifyRender();
        }

        return true;
    }
} // Slab::Graphics