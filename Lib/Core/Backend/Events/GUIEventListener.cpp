//
// Created by joao on 02/09/2021.
//

#include "GUIEventListener.h"


#define EVENT_CALLBACK(FUNC) { \
    auto responded = false;    \
    for(auto &r : unmanagedDelegateResponders) if(r->FUNC) responded = true; \
    return responded;          \
    };

Core::GUIEventListener::GUIEventListener() { }

void Core::GUIEventListener::addResponder(Core::GUIEventListener *responder) {
    unmanagedDelegateResponders.emplace_back(responder);
}

bool Core::GUIEventListener::hasResponders() const {
    return ! unmanagedDelegateResponders.empty();
}


bool Core::GUIEventListener::notifyKeyboard(unsigned char key, int x, int y)               EVENT_CALLBACK(notifyKeyboard(key, x, y))
bool Core::GUIEventListener::notifyKeyboardSpecial(int key, int x, int y)                  EVENT_CALLBACK(notifyKeyboardSpecial(key, x, y))
bool Core::GUIEventListener::notifyMouseButton(int button, int dir, int x, int y)          EVENT_CALLBACK(notifyMouseButton(button, dir, x, y))
bool Core::GUIEventListener::notifyMouseMotion(int x, int y)                               EVENT_CALLBACK(notifyMouseMotion(x, y))
bool Core::GUIEventListener::notifyMousePassiveMotion(int x, int y)                        EVENT_CALLBACK(notifyMousePassiveMotion(x, y))
bool Core::GUIEventListener::notifyMouseWheel(int wheel, int direction, int x, int y)      EVENT_CALLBACK(notifyMouseWheel(wheel, direction, x, y))
bool Core::GUIEventListener::notifyScreenReshape(int newScreenWidth, int newScreenHeight)  EVENT_CALLBACK(notifyScreenReshape(newScreenWidth, newScreenHeight))
bool Core::GUIEventListener::notifyRender(float elTime_msec)                               EVENT_CALLBACK(notifyRender(elTime_msec))

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