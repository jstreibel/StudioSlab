//
// Created by joao on 02/09/2021.
//

#include "SystemWindowEventListener.h"

#include <crude_json.h>
#include <Core/Tools/Log.h>

#include "Utils/ReferenceIterator.h"

#include "Graphics/Backend/SystemWindow.h"

namespace Slab::Graphics {

    SystemWindowEventListener::SystemWindowEventListener() : parent_system_window(nullptr) { }

    SystemWindowEventListener::SystemWindowEventListener(ParentSystemWindow parent)
    : parent_system_window(parent) {}

    SystemWindowEventListener::~SystemWindowEventListener() = default;

    void SystemWindowEventListener::setParentSystemWindow(SystemWindow* syswin) {
        parent_system_window = syswin;

        notifySystemWindowReshape(syswin->getWidth(), syswin->getHeight());
    }

    void SystemWindowEventListener::addResponder(const Volatile<SystemWindowEventListener>& responder) {
        assert(responder.lock().get() != this);

        delegateResponders.emplace_back(responder);
    }

    bool SystemWindowEventListener::hasResponders() const {
        return !delegateResponders.empty();
    }

    void SystemWindowEventListener::removeResponder(const Pointer<SystemWindowEventListener>& to_remove) {
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
        return IterateReferences(delegateResponders, Func(notifyMouseMotion, x, y, dx, dy));
    }

    bool SystemWindowEventListener::notifySystemWindowReshape(int w, int h) {
        return IterateReferences(delegateResponders, Func(notifySystemWindowReshape, w, h));
    }

    bool SystemWindowEventListener::notifyRender(){
        return IterateReferences(delegateResponders, Func(notifyRender));
    }

    bool SystemWindowEventListener::notifyKeyboard(KeyMap key,
                                                   KeyState state,
                                                   ModKeys modKeys){
        return IterateReferences(delegateResponders, Func(notifyKeyboard, key, state, modKeys));
    }

    bool SystemWindowEventListener::notifyCharacter(UInt codepoint) {
        return IterateReferences(delegateResponders, Func(notifyCharacter, codepoint));
    }

    bool SystemWindowEventListener::
    notifyMouseButton(MouseButton button,
                      KeyState state,
                      ModKeys mods) {
        return IterateReferences(delegateResponders, Func(notifyMouseButton, button, state, mods));
    }


    bool SystemWindowEventListener::
    notifyMouseWheel(double dx, double dy) {
        return IterateReferences(delegateResponders, Func(notifyMouseWheel, dx, dy));
    }

    bool SystemWindowEventListener::
    notifyFilesDropped(StrVector paths) {
        return IterateReferences(delegateResponders, Func(notifyFilesDropped, paths));
    }
}