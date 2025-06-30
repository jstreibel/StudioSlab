//
// Created by joao on 02/09/2021.
//

#include "SystemWindowEventListener.h"

// #include <crude_json.h>
#include <Core/Tools/Log.h>

#include "Utils/ReferenceIterator.h"

#include "Graphics/Backend/SystemWindow.h"

namespace Slab::Graphics {

    FSystemWindowEventListener::FSystemWindowEventListener() : parent_system_window(nullptr) { }

    FSystemWindowEventListener::FSystemWindowEventListener(FOwnerSystemWindow parent)
    : parent_system_window(parent) {}

    FSystemWindowEventListener::~FSystemWindowEventListener() = default;

    void FSystemWindowEventListener::SetParentSystemWindow(SystemWindow* syswin) {
        parent_system_window = syswin;

        NotifySystemWindowReshape(syswin->GetWidth(), syswin->GetHeight());
    }

    void FSystemWindowEventListener::AddResponder(const Volatile<FSystemWindowEventListener>& responder) {
        assert(responder.lock().get() != this);

        delegateResponders.emplace_back(responder);
    }

    bool FSystemWindowEventListener::HasResponders() const {
        return !delegateResponders.empty();
    }

    void FSystemWindowEventListener::RemoveResponder(const Pointer<FSystemWindowEventListener>& to_remove) {
        auto responder = delegateResponders.begin();

        while(responder != delegateResponders.end()) {
            if(responder->lock() == to_remove)
                responder = delegateResponders.erase(responder);
            else
                ++responder;
        }

    }

    void FSystemWindowEventListener::CursorEntered(bool entered) {
        IterateReferences(delegateResponders, FuncRun(CursorEntered, entered));
    }

    bool FSystemWindowEventListener::NotifyMouseMotion(int x, int y, int dx, int dy) {
        return IterateReferences(delegateResponders, Func(NotifyMouseMotion, x, y, dx, dy));
    }

    bool FSystemWindowEventListener::NotifySystemWindowReshape(int w, int h) {
        return IterateReferences(delegateResponders, Func(NotifySystemWindowReshape, w, h));
    }

    bool FSystemWindowEventListener::NotifyRender(){
        return IterateReferences(delegateResponders, Func(NotifyRender));
    }

    bool FSystemWindowEventListener::NotifyKeyboard(EKeyMap key,
                                                   EKeyState state,
                                                   EModKeys modKeys){
        return IterateReferences(delegateResponders, Func(NotifyKeyboard, key, state, modKeys));
    }

    bool FSystemWindowEventListener::NotifyCharacter(UInt codepoint) {
        return IterateReferences(delegateResponders, Func(NotifyCharacter, codepoint));
    }

    bool FSystemWindowEventListener::
    NotifyMouseButton(EMouseButton button,
                      EKeyState state,
                      EModKeys mods) {
        return IterateReferences(delegateResponders, Func(NotifyMouseButton, button, state, mods));
    }


    bool FSystemWindowEventListener::
    NotifyMouseWheel(double dx, double dy) {
        return IterateReferences(delegateResponders, Func(NotifyMouseWheel, dx, dy));
    }

    bool FSystemWindowEventListener::
    NotifyFilesDropped(StrVector paths) {
        return IterateReferences(delegateResponders, Func(NotifyFilesDropped, paths));
    }
}