//
// Created by joao on 7/9/24.
//

#include "WindowProxy.h"

namespace Slab {
    namespace Graphics {
        WindowProxy::WindowProxy() : Window() {

        }

        void WindowProxy::setWindow(Pointer<Window> win) {
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

        bool WindowProxy::notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) {
            if(window == nullptr) return false;

            return window->notifyMouseButton(button, state, keys);
        }

        bool WindowProxy::notifyRender() {
            if(window == nullptr) return false;

            return window->notifyRender();
        }

        bool WindowProxy::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {
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

        bool WindowProxy::notifyFilesDropped(StrVector paths) {
            if(window == nullptr) return false;

            return window->notifyFilesDropped(paths);
        }
    } // Slab
} // Graphics