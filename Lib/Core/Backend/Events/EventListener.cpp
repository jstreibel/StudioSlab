//
// Created by joao on 02/09/2021.
//

#include "EventListener.h"


#define EVENT_CALLBACK(FUNC) { \
    auto responded = false;    \
    for(auto &r : unmanagedDelegateResponders) if(r->FUNC) responded = true; \
    return responded;          \
    };

Core::EventListener::EventListener() { }

void Core::EventListener::addResponder(Core::EventListener *responder) {
    unmanagedDelegateResponders.emplace_back(responder);
}

bool Core::EventListener::hasResponders() const {
    return ! unmanagedDelegateResponders.empty();
}


bool Core::EventListener::notifyKeyboard(unsigned char key, int x, int y)               EVENT_CALLBACK(notifyKeyboard(key, x, y))
bool Core::EventListener::notifyKeyboardSpecial(int key, int x, int y)                  EVENT_CALLBACK(notifyKeyboardSpecial(key, x, y))
bool Core::EventListener::notifyMouseButton(int button, int dir, int x, int y)          EVENT_CALLBACK(notifyMouseButton(button, dir, x, y))
bool Core::EventListener::notifyMouseMotion(int x, int y)                               EVENT_CALLBACK(notifyMouseMotion(x, y))
bool Core::EventListener::notifyMousePassiveMotion(int x, int y)                        EVENT_CALLBACK(notifyMousePassiveMotion(x, y))
bool Core::EventListener::notifyMouseWheel(int wheel, int direction, int x, int y)      EVENT_CALLBACK(notifyMouseWheel(wheel, direction, x, y))
bool Core::EventListener::notifyScreenReshape(int newScreenWidth, int newScreenHeight)  EVENT_CALLBACK(notifyScreenReshape(newScreenWidth, newScreenHeight))
bool Core::EventListener::notifyRender(float elTime_msec)                               EVENT_CALLBACK(notifyRender(elTime_msec))