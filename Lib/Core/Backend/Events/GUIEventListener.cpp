//
// Created by joao on 02/09/2021.
//

#include "GUIEventListener.h"


#define EVENT_CALLBACK(FUNC) { \
    auto responded = false;    \
    for(auto &r : unmanagedDelegateResponders) if(r->FUNC) responded = true; \
    return responded;          \
    };

Core::GUIEventListener::GUIEventListener() = default;

void Core::GUIEventListener::addResponder(Core::GUIEventListener *responder) {
    assert(responder != this);
    unmanagedDelegateResponders.emplace_back(responder);
}

bool Core::GUIEventListener::hasResponders() const {
    return ! unmanagedDelegateResponders.empty();
}


bool Core::GUIEventListener::notifyMouseMotion(int x, int y)                               EVENT_CALLBACK(notifyMouseMotion(x, y))
bool Core::GUIEventListener::notifyScreenReshape(int w, int h)                             EVENT_CALLBACK(notifyScreenReshape(w, h))
bool Core::GUIEventListener::notifyRender()                                                EVENT_CALLBACK(notifyRender())

bool Core::GUIEventListener::
notifyKeyboard(Core::KeyMap key,
               Core::KeyState state,
               Core::ModKeys modKeys)          EVENT_CALLBACK(notifyKeyboard(key, state, modKeys))

bool Core::GUIEventListener::
notifyMouseButton(Core::MouseButton button,
                  Core::KeyState state,
                  Core::ModKeys mods)          EVENT_CALLBACK(notifyMouseButton(button, state, mods))



bool Core::GUIEventListener::
notifyMouseWheel(double dx, double dy)         EVENT_CALLBACK(notifyMouseWheel(dx, dy))



bool Core::GUIEventListener::
notifyFilesDropped(StrVector paths)            EVENT_CALLBACK(notifyFilesDropped(paths))