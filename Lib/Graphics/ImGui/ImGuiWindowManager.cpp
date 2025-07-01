//
// Created by joao on 6/28/25.
//

#include "ImGuiWindowManager.h"

#include "Graphics/SlabGraphics.h"

namespace Slab::Graphics {

    FImGuiWindowManager::FImGuiWindowManager(const Pointer<FImGuiContext>& Context)
    : Context(Context)
    {
        if (this->Context == nullptr)
        {
            auto GUIContext = GetGraphicsBackend()->GetMainSystemWindow()->GetGUIContext();
            this->Context = DynamicPointerCast<FImGuiContext>(GUIContext);
        }
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