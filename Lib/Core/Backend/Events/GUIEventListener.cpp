//
// Created by joao on 02/09/2021.
//

#include "GUIEventListener.h"


#define EVENT_CALLBACK(FUNC) { \
    auto responded = false;    \
    for(auto &r : unmanagedDelegateResponders) if(r->FUNC) responded = true; \
    return responded;          \
    };


namespace Slab::Core {


    GUIEventListener::GUIEventListener() = default;

    void GUIEventListener::addResponder(GUIEventListener *responder) {
        assert(responder != this);
        unmanagedDelegateResponders.emplace_back(responder);
    }

    bool GUIEventListener::hasResponders() const {
        return !unmanagedDelegateResponders.empty();
    }


    bool GUIEventListener::notifyMouseMotion(int x, int y) EVENT_CALLBACK(notifyMouseMotion(x, y))

    bool GUIEventListener::notifyScreenReshape(int w, int h) EVENT_CALLBACK(notifyScreenReshape(w, h))

    bool GUIEventListener::notifyRender() EVENT_CALLBACK(notifyRender())

    bool GUIEventListener::
    notifyKeyboard(KeyMap key,
                   KeyState state,
                   ModKeys modKeys) EVENT_CALLBACK(notifyKeyboard(key, state, modKeys))

    bool GUIEventListener::
    notifyMouseButton(MouseButton button,
                      KeyState state,
                      ModKeys mods) EVENT_CALLBACK(notifyMouseButton(button, state, mods))


    bool GUIEventListener::
    notifyMouseWheel(double dx, double dy) EVENT_CALLBACK(notifyMouseWheel(dx, dy))


    bool GUIEventListener::
    notifyFilesDropped(StrVector paths) EVENT_CALLBACK(notifyFilesDropped(paths))

}