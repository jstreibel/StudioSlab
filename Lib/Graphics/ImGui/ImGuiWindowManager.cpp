//
// Created by joao on 6/28/25.
//

#include "ImGuiWindowManager.h"

#include "Graphics/SlabGraphics.h"

namespace Slab::Graphics {

    FImGuiWindowManager::FImGuiWindowManager(const TPointer<FImGuiContext>& Context)
    : Context(Context)
    {
        if (this->Context == nullptr)
        {
            throw Exception("ImGuiContext is null");
        }
    }

    void FImGuiWindowManager::AddSlabWindow(const TPointer<FSlabWindow>& SlabWindow, bool Hidden)
    {
        auto WrapperWindow = New<FSlabWindow_ImGuiWrapper>(SlabWindow, Context);
        Windows.emplace_back(WrapperWindow);
    }

    bool FImGuiWindowManager::NotifyRender(const FPlatformWindow& PlatformWindow)
    {
        // **********************
        // Important!
        // We don't want to call FWindowManager::NotifyRender(const FPlatformWindow&);
        // this is because, while 'Context' is this class's delegate responder, we don't
        // want it to be the delegate responder to NotifyRender

        for (const auto& WrapperWindow : Windows)
        {
            WrapperWindow->NotifyRender(PlatformWindow);
        }

        return true;
    }
} // Slab::Graphics