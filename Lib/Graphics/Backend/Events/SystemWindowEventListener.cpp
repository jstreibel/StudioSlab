//
// Created by joao on 02/09/2021.
//

#include "SystemWindowEventListener.h"

#include <crude_json.h>
#include <Core/Tools/Log.h>

#include "Utils/ReferenceIterator.h"

#include "Graphics/Backend/SystemWindow.h"

namespace Slab::Graphics {

    FSystemWindowEventListener::FSystemWindowEventListener() : parent_system_window(nullptr) { }

    FSystemWindowEventListener::FSystemWindowEventListener(ParentSystemWindow parent)
    : parent_system_window(parent) {}

    FSystemWindowEventListener::~FSystemWindowEventListener() = default;

    void FSystemWindowEventListener::setParentSystemWindow(SystemWindow* syswin) {
        parent_system_window = syswin;

        notifySystemWindowReshape(syswin->getWidth(), syswin->getHeight());
    }

    void FSystemWindowEventListener::addResponder(const Volatile<FSystemWindowEventListener>& responder) {
        assert(responder.lock().get() != this);

        delegateResponders.emplace_back(responder);
    }

    bool FSystemWindowEventListener::hasResponders() const {
        return !delegateResponders.empty();
    }

    void FSystemWindowEventListener::removeResponder(const Pointer<FSystemWindowEventListener>& to_remove) {
        auto responder = delegateResponders.begin();

        while(responder != delegateResponders.end()) {
            if(responder->lock() == to_remove)
                responder = delegateResponders.erase(responder);
            else
                ++responder;
        }

    }

    void FSystemWindowEventListener::cursorEntered(bool entered) {
        IterateReferences(delegateResponders, FuncRun(cursorEntered, entered));
    }

    bool FSystemWindowEventListener::notifyMouseMotion(int x, int y, int dx, int dy) {
        return IterateReferences(delegateResponders, Func(notifyMouseMotion, x, y, dx, dy));
    }

    bool FSystemWindowEventListener::notifySystemWindowReshape(int w, int h) {
        return IterateReferences(delegateResponders, Func(notifySystemWindowReshape, w, h));
    }

    bool FSystemWindowEventListener::notifyRender(){
        return IterateReferences(delegateResponders, Func(notifyRender));
    }

    bool FSystemWindowEventListener::NotifyKeyboard(KeyMap key,
                                                   KeyState state,
                                                   ModKeys modKeys){
        return IterateReferences(delegateResponders, Func(NotifyKeyboard, key, state, modKeys));
    }

    bool FSystemWindowEventListener::notifyCharacter(UInt codepoint) {
        return IterateReferences(delegateResponders, Func(notifyCharacter, codepoint));
    }

    bool FSystemWindowEventListener::
    NotifyMouseButton(MouseButton button,
                      KeyState state,
                      ModKeys mods) {
        return IterateReferences(delegateResponders, Func(NotifyMouseButton, button, state, mods));
    }


    bool FSystemWindowEventListener::
    notifyMouseWheel(double dx, double dy) {
        return IterateReferences(delegateResponders, Func(notifyMouseWheel, dx, dy));
    }

    bool FSystemWindowEventListener::
    notifyFilesDropped(StrVector paths) {
        return IterateReferences(delegateResponders, Func(notifyFilesDropped, paths));
    }
}