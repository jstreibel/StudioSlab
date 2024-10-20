//
// Created by joao on 10/19/24.
//

#include "WindowManager.h"

namespace Slab {
    namespace Graphics {
        void WindowManager::addSlabWindow(Pointer<Window> slab_window) {
            slab_windows.push_back(slab_window);

            if(focused == nullptr) focused = slab_window;
        }

        bool WindowManager::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
            if(focused == nullptr) return false;

            return focused->notifyKeyboard(key, state, modKeys);
        }

        bool WindowManager::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
            if(focused == nullptr) return false;

            return focused->notifyMouseButton(button, state, keys);
        }

        bool WindowManager::notifyMouseMotion(int x, int y) {
            if(focused == nullptr) return false;

            return focused->notifyMouseMotion(x, y);
        }

        bool WindowManager::notifyMouseWheel(double dx, double dy) {
            if(focused == nullptr) return false;

            return focused->notifyMouseWheel(dx, dy);
        }

        bool WindowManager::notifyFilesDropped(StrVector paths) {
            if(focused == nullptr) return false;

            return focused->notifyFilesDropped(paths);
        }

        bool WindowManager::notifySystemWindowReshape(int w, int h) {
            if(focused == nullptr) return false;

            return focused->notifySystemWindowReshape(w, h);
        }

        bool WindowManager::notifyRender() {
            if(focused == nullptr) return false;

            return focused->notifyRender();
        }

    } // Slab
} // Graphics