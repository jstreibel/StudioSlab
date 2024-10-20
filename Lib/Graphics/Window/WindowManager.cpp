//
// Created by joao on 10/19/24.
//

#include "SlabWindowManager.h"
#include "Decorator.h"

namespace Slab::Graphics {


    void SlabWindowManager::addSlabWindow(Pointer<SlabWindow> slab_window) {
        slab_windows.push_back(slab_window);

        if(focused == nullptr) focused = slab_window;
    }

    bool SlabWindowManager::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(focused == nullptr) return false;

        return focused->notifyKeyboard(key, state, modKeys);
    }

    bool SlabWindowManager::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if(focused == nullptr) return false;

        return focused->notifyMouseButton(button, state, keys);
    }

    bool SlabWindowManager::notifyMouseMotion(int x, int y) {
        if(focused == nullptr) return false;

        return focused->notifyMouseMotion(x, y);
    }

    bool SlabWindowManager::notifyMouseWheel(double dx, double dy) {
        if(focused == nullptr) return false;

        return focused->notifyMouseWheel(dx, dy);
    }

    bool SlabWindowManager::notifyFilesDropped(StrVector paths) {
        if(focused == nullptr) return false;

        return false;
    }

    bool SlabWindowManager::notifySystemWindowReshape(int w, int h) {
        if(focused == nullptr) return false;

        for(auto &slab_window : slab_windows) if(slab_window->isFullscreen()) slab_window->notifyReshape(w, h);

        return false;
    }

    bool SlabWindowManager::notifyRender() {
        if(focused == nullptr) return false;

        static Decorator decorate;

        for(auto &slab_window : slab_windows) {
            decorate(*slab_window);
            slab_window->draw();
        }

        return true;
    }


} // Slab::Graphics