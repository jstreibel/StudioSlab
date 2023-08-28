//
// Created by joao on 02/09/2021.
//

#include "EventListener.h"


#define EVENT_CALLBACK(FUNC) { \
    auto responded = false;    \
    for(auto &r : unmanagedDelegateResponders) if(r->FUNC) responded = true; \
    return responded;          \
    };

Base::EventListener::EventListener() { }

void Base::EventListener::addResponder(Base::EventListener *responder) {
    unmanagedDelegateResponders.emplace_back(responder);
}

bool Base::EventListener::hasResponders() const {
    return ! unmanagedDelegateResponders.empty();
}


bool Base::EventListener::notifyKeyboard(unsigned char key, int x, int y)               EVENT_CALLBACK(notifyKeyboard(key, x, y))
bool Base::EventListener::notifyKeyboardSpecial(int key, int x, int y)                  EVENT_CALLBACK(notifyKeyboardSpecial(key, x, y))
bool Base::EventListener::notifyMouseButton(int button, int dir, int x, int y)          EVENT_CALLBACK(notifyMouseButton(button, dir, x, y))
bool Base::EventListener::notifyMouseMotion(int x, int y)                               EVENT_CALLBACK(notifyMouseMotion(x, y))
bool Base::EventListener::notifyMousePassiveMotion(int x, int y)                        EVENT_CALLBACK(notifyMousePassiveMotion(x, y))
bool Base::EventListener::notifyMouseWheel(int wheel, int direction, int x, int y)      EVENT_CALLBACK(notifyMouseWheel(wheel, direction, x, y))
bool Base::EventListener::notifyScreenReshape(int newScreenWidth, int newScreenHeight)  EVENT_CALLBACK(notifyScreenReshape(newScreenWidth, newScreenHeight))
bool Base::EventListener::notifyRender(float elTime_msec)                               EVENT_CALLBACK(notifyRender(elTime_msec))