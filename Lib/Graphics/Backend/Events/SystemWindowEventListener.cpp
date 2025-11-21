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

    FPlatformWindowEventListener::FPlatformWindowEventListener( )
    : Priority(0)
    {
    }

    FPlatformWindowEventListener::~FPlatformWindowEventListener() = default;

    /*
    void FPlatformWindowEventListener::SetParentPlatformWindow(const FOwnerPlatformWindow Parent) {
        w_ParentPlatformWindow = Parent;

        if (const auto ParentPtr = Parent.lock())
        {
            NotifySystemWindowReshape(ParentPtr->GetWidth(), ParentPtr->GetHeight());
        }
    }*/

    void FPlatformWindowEventListener::AddResponder(const TVolatile<FPlatformWindowEventListener>& responder) {
        assert(responder.lock().get() != this);

        DelegateResponders.emplace_back(responder);
    }

    bool FPlatformWindowEventListener::HasResponders() const {
        return !DelegateResponders.empty();
    }

    void FPlatformWindowEventListener::RemoveResponder(const TPointer<FPlatformWindowEventListener>& to_remove) {
        auto responder = DelegateResponders.begin();

        while(responder != DelegateResponders.end()) {
            if(responder->lock() == to_remove)
                responder = DelegateResponders.erase(responder);
            else
                ++responder;
        }

    }

    void FPlatformWindowEventListener::CursorEntered(bool entered) {
        IterateReferences(DelegateResponders, SLAB_FUNC_RUNTHROUGH(CursorEntered, entered));
    }

    bool FPlatformWindowEventListener::NotifyMouseMotion(int x, int y, int dx, int dy) {
        return IterateReferences(DelegateResponders, SLAB_FUNC(NotifyMouseMotion, x, y, dx, dy));
    }

    bool FPlatformWindowEventListener::NotifySystemWindowReshape(int w, int h) {
        return IterateReferences(DelegateResponders, SLAB_FUNC(NotifySystemWindowReshape, w, h));
    }

    bool FPlatformWindowEventListener::NotifyRender(const FPlatformWindow& PlatformWindow){
        auto Funky = [&PlatformWindow] (const auto &obj) {
            return obj->NotifyRender(PlatformWindow);
        };

        return IterateReferences(DelegateResponders, Funky);
    }

    bool FPlatformWindowEventListener::NotifyKeyboard(EKeyMap key,
                                                   EKeyState state,
                                                   EModKeys modKeys){
        return IterateReferences(DelegateResponders, SLAB_FUNC(NotifyKeyboard, key, state, modKeys));
    }

    bool FPlatformWindowEventListener::NotifyCharacter(UInt codepoint) {
        return IterateReferences(DelegateResponders, SLAB_FUNC(NotifyCharacter, codepoint));
    }

    bool FPlatformWindowEventListener::
    NotifyMouseButton(EMouseButton button,
                      EKeyState state,
                      EModKeys mods) {
        return IterateReferences(DelegateResponders, SLAB_FUNC(NotifyMouseButton, button, state, mods));
    }


    bool FPlatformWindowEventListener::
    NotifyMouseWheel(double dx, double dy) {
        return IterateReferences(DelegateResponders, SLAB_FUNC(NotifyMouseWheel, dx, dy));
    }

    bool FPlatformWindowEventListener::
    NotifyFilesDropped(StrVector paths) {
        return IterateReferences(DelegateResponders, SLAB_FUNC(NotifyFilesDropped, paths));
    }
}