//
// Created by joao on 7/9/24.
//

#include "WindowProxy.h"

namespace Slab::Graphics {
    WindowProxy::WindowProxy() : SlabWindow() {

    }

    void WindowProxy::setWindow(Pointer<SlabWindow> win) {
        window = win;

        // window->setx(getx());
        // window->sety(gety());
        window->notifyReshape(getw(), geth());
    }

    void WindowProxy::draw() {
        if(window == nullptr) return;

        window->draw();
    }

    void WindowProxy::notifyReshape(int w, int h) {
        if(window == nullptr) return;

        window->notifyReshape(w, h);
    }

    bool WindowProxy::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if(window == nullptr) return false;

        return window->notifyMouseButton(button, state, keys);
    }

    bool WindowProxy::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(window == nullptr) return false;

        return window->notifyKeyboard(key, state, modKeys);
    }

    bool WindowProxy::notifyMouseMotion(int x, int y) {
        if(window == nullptr) return false
        ;
        return window->notifyMouseMotion(x, y);
    }

    bool WindowProxy::notifyMouseWheel(double dx, double dy) {
        if(window == nullptr) return false;

        return window->notifyMouseWheel(dx, dy);
    }

} // Slab::Graphics