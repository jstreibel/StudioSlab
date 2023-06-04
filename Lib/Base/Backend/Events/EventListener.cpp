//
// Created by joao on 02/09/2021.
//

#include "EventListener.h"


Base::EventListener::EventListener(bool allowDelegateResponders) : allowsDelegateResponders(allowDelegateResponders) { }

void Base::EventListener::addResponder(Base::EventListener *responder) {
    if(!allowsDelegateResponders) throw "This EventListener does not allow delegate responders.";
    unmanagedDelegateResponders.emplace_back(responder);
}

bool Base::EventListener::hasResponders() const {
    if(!allowsDelegateResponders) return false;
    return ! unmanagedDelegateResponders.empty();
}

bool Base::EventListener::notifyKeyboard(unsigned char key, int x, int y) {                 auto responded = false; for(auto &r : unmanagedDelegateResponders) if(r->notifyKeyboard(key, x, y))                            responded = true; return responded; }
bool Base::EventListener::notifyKeyboardSpecial(int key, int x, int y) {                    auto responded = false; for(auto &r : unmanagedDelegateResponders) if(r->notifyKeyboardSpecial(key, x, y))                     responded = true; return responded; }

bool Base::EventListener::notifyMouseButton(int button, int dir, int x, int y) {            auto responded = false; for(auto &r : unmanagedDelegateResponders) if(r->notifyMouseButton(button, dir, x, y))                 responded = true; return responded; }
bool Base::EventListener::notifyMouseMotion(int x, int y) {                                 auto responded = false; for(auto &r : unmanagedDelegateResponders) if(r->notifyMouseMotion(x, y))                              responded = true; return responded; }
bool Base::EventListener::notifyMousePassiveMotion(int x, int y) {                          auto responded = false; for(auto &r : unmanagedDelegateResponders) if(r->notifyMousePassiveMotion(x, y))                       responded = true; return responded; }
bool Base::EventListener::notifyMouseWheel(int wheel, int direction, int x, int y) {        auto responded = false; for(auto &r : unmanagedDelegateResponders) if(r->notifyMouseWheel(wheel, direction, x, y))             responded = true; return responded; }

bool Base::EventListener::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {    auto responded = false; for(auto &r : unmanagedDelegateResponders) if(r->notifyScreenReshape(newScreenWidth, newScreenHeight)) responded = true; return responded; }

bool Base::EventListener::notifyRender() {                                                  auto responded = false; for(auto &r : unmanagedDelegateResponders) if(r->notifyRender())                                       responded = true; return responded; }

