//
// Created by joao on 02/09/2021.
//

#include "SystemWindowEventListener.h"

// #include <crude_json.h>
#include <Core/Tools/Log.h>

#include "Graphics/SlabGraphics.h"
#include "Utils/ReferenceIterator.h"

#include "Graphics/Backend/PlatformWindow.h"

namespace Slab::Graphics {

    FPlatformWindowEventListener::FPlatformWindowEventListener(const FOwnerPlatformWindow& ParentPlatformWindow)
    : Priority(0), w_ParentPlatformWindow(ParentPlatformWindow)
    {
    }

    FPlatformWindowEventListener::~FPlatformWindowEventListener() = default;

    void FPlatformWindowEventListener::SetParentPlatformWindow(const FOwnerPlatformWindow Parent) {
        w_ParentPlatformWindow = Parent;

        if (const auto ParentPtr = Parent.lock())
        {
            NotifySystemWindowReshape(ParentPtr->GetWidth(), ParentPtr->GetHeight());
        }
    }

    void FPlatformWindowEventListener::AddResponder(const TVolatile<FPlatformWindowEventListener>& responder) {
        assert(responder.lock().get() != this);

        delegateResponders.emplace_back(responder);
    }

    bool FPlatformWindowEventListener::HasResponders() const {
        return !delegateResponders.empty();
    }

    void FPlatformWindowEventListener::RemoveResponder(const Pointer<FPlatformWindowEventListener>& to_remove) {
        auto responder = delegateResponders.begin();

        while(responder != delegateResponders.end()) {
            if(responder->lock() == to_remove)
                responder = delegateResponders.erase(responder);
            else
                ++responder;
        }

    }

    void FPlatformWindowEventListener::CursorEntered(bool entered) {
        IterateReferences(delegateResponders, FuncRun(CursorEntered, entered));
    }

    bool FPlatformWindowEventListener::NotifyMouseMotion(int x, int y, int dx, int dy) {
        return IterateReferences(delegateResponders, Func(NotifyMouseMotion, x, y, dx, dy));
    }

    bool FPlatformWindowEventListener::NotifySystemWindowReshape(int w, int h) {
        return IterateReferences(delegateResponders, Func(NotifySystemWindowReshape, w, h));
    }

    bool FPlatformWindowEventListener::NotifyRender(){
        return IterateReferences(delegateResponders, Func(NotifyRender));
    }

    bool FPlatformWindowEventListener::NotifyKeyboard(EKeyMap key,
                                                   EKeyState state,
                                                   EModKeys modKeys){
        return IterateReferences(delegateResponders, Func(NotifyKeyboard, key, state, modKeys));
    }

    bool FPlatformWindowEventListener::NotifyCharacter(UInt codepoint) {
        return IterateReferences(delegateResponders, Func(NotifyCharacter, codepoint));
    }

    bool FPlatformWindowEventListener::
    NotifyMouseButton(EMouseButton button,
                      EKeyState state,
                      EModKeys mods) {
        return IterateReferences(delegateResponders, Func(NotifyMouseButton, button, state, mods));
    }


    bool FPlatformWindowEventListener::
    NotifyMouseWheel(double dx, double dy) {
        return IterateReferences(delegateResponders, Func(NotifyMouseWheel, dx, dy));
    }

    bool FPlatformWindowEventListener::
    NotifyFilesDropped(StrVector paths) {
        return IterateReferences(delegateResponders, Func(NotifyFilesDropped, paths));
    }
}