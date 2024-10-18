//
// Created by joao on 02/09/2021.
//

#include "GUIEventListener.h"
#include "Utils/ReferenceIterator.h"


namespace Slab::Core {

    GUIEventListener::GUIEventListener() = default;

    void GUIEventListener::addResponder(Volatile<GUIEventListener> responder) {
        assert(responder.lock().get() != this);

        delegateResponders.emplace_back(responder);
    }

    bool GUIEventListener::hasResponders() const {
        return !delegateResponders.empty();
    }

    void GUIEventListener::removeResponder(Pointer<GUIEventListener> to_remove) {
        auto responder = delegateResponders.begin();

        while(responder != delegateResponders.end()) {
            if(responder->lock() == to_remove)
                responder = delegateResponders.erase(responder);
            else
                ++responder;
        }

    }


    bool GUIEventListener::notifyMouseMotion(int x, int y) {
        return IterateReferences(delegateResponders, FuncRun(notifyMouseMotion, x, y));
    }

    bool GUIEventListener::notifyScreenReshape(int w, int h) {
        return IterateReferences(delegateResponders, FuncRun(notifyScreenReshape, w, h));
    }

    bool GUIEventListener::notifyRender(){
        return IterateReferences(delegateResponders, FuncRun(notifyRender));
    }

    bool GUIEventListener::
    notifyKeyboard(KeyMap key,
                   KeyState state,
                   ModKeys modKeys){
        return IterateReferences(delegateResponders, FuncRun(notifyKeyboard, key, state, modKeys));
    }

    bool GUIEventListener::
    notifyMouseButton(MouseButton button,
                      KeyState state,
                      ModKeys mods) {
        return IterateReferences(delegateResponders, FuncRun(notifyMouseButton, button, state, mods));
    }


    bool GUIEventListener::
    notifyMouseWheel(double dx, double dy) {
        return IterateReferences(delegateResponders, FuncRun(notifyMouseWheel, dx, dy));
    }

    bool GUIEventListener::
    notifyFilesDropped(StrVector paths) {
        return IterateReferences(delegateResponders, FuncRun(notifyFilesDropped, paths));
    }
}