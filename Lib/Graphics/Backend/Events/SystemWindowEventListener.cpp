//
// Created by joao on 02/09/2021.
//

#include "SystemWindowEventListener.h"
#include "Utils/ReferenceIterator.h"

#include "Graphics/Backend/SystemWindow.h"

namespace Slab::Graphics {

    SystemWindowEventListener::SystemWindowEventListener() = default;

    void SystemWindowEventListener::setParentSystemWindow(SystemWindow* syswin) {
        parent_system_window = syswin;

        notifySystemWindowReshape(syswin->getWidth(), syswin->getHeight());
    }

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

    void SystemWindowEventListener::cursorEntered(bool entered) {
        IterateReferences(delegateResponders, FuncRun(cursorEntered, entered));
    }

    bool SystemWindowEventListener::notifyMouseMotion(int x, int y, int dx, int dy) {
        return IterateReferences(delegateResponders, FuncRun(notifyMouseMotion, x, y, dx, dy));
    }

    bool SystemWindowEventListener::notifySystemWindowReshape(int w, int h) {
        return IterateReferences(delegateResponders, FuncRun(notifySystemWindowReshape, w, h));
    }

    bool SystemWindowEventListener::notifyRender(){
        return IterateReferences(delegateResponders, FuncRun(notifyRender));
    }

    bool SystemWindowEventListener::notifyKeyboard(KeyMap key,
                                                   KeyState state,
                                                   ModKeys modKeys){
        return IterateReferences(delegateResponders, FuncRun(notifyKeyboard, key, state, modKeys));
    }

    bool SystemWindowEventListener::notifyCharacter(UInt codepoint) {
        return IterateReferences(delegateResponders, FuncRun(notifyCharacter, codepoint));
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