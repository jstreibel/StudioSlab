//
// Created by joao on 02/09/2021.
//

#include "SystemWindowEventListener.h"
#include "Utils/ReferenceIterator.h"


namespace Slab::Graphics {

    SystemWindowEventListener::SystemWindowEventListener() = default;

    void SystemWindowEventListener::addResponder(Volatile<SystemWindowEventListener> responder) {
        assert(responder.lock().get() != this);

        delegateResponders.emplace_back(responder);
    }

    bool SystemWindowEventListener::hasResponders() const {
        return !delegateResponders.empty();
    }

    void SystemWindowEventListener::removeResponder(Pointer<SystemWindowEventListener> to_remove) {
        auto responder = delegateResponders.begin();

        while(responder != delegateResponders.end()) {
            if(responder->lock() == to_remove)
                responder = delegateResponders.erase(responder);
            else
                ++responder;
        }

    }


    bool SystemWindowEventListener::notifyMouseMotion(int x, int y) {
        return IterateReferences(delegateResponders, FuncRun(notifyMouseMotion, x, y));
    }

    bool SystemWindowEventListener::notifySystemWindowReshape(int w, int h) {
        return IterateReferences(delegateResponders, FuncRun(notifySystemWindowReshape, w, h));
    }

    bool SystemWindowEventListener::notifyRender(){
        return IterateReferences(delegateResponders, FuncRun(notifyRender));
    }

    bool SystemWindowEventListener::
    notifyKeyboard(KeyMap key,
                   KeyState state,
                   ModKeys modKeys){
        return IterateReferences(delegateResponders, FuncRun(notifyKeyboard, key, state, modKeys));
    }

    bool SystemWindowEventListener::
    notifyMouseButton(MouseButton button,
                      KeyState state,
                      ModKeys mods) {
        return IterateReferences(delegateResponders, FuncRun(notifyMouseButton, button, state, mods));
    }


    bool SystemWindowEventListener::
    notifyMouseWheel(double dx, double dy) {
        return IterateReferences(delegateResponders, FuncRun(notifyMouseWheel, dx, dy));
    }

    bool SystemWindowEventListener::
    notifyFilesDropped(StrVector paths) {
        return IterateReferences(delegateResponders, FuncRun(notifyFilesDropped, paths));
    }
}