//
// Created by joao on 10/17/24.
//

#include "SystemWindow.h"

namespace Slab::Graphics {

    void SystemWindow::addEventListener(Pointer<SystemWindowEventListener> listener) {
        try {
            auto window = DynamicPointerCast<Window>(listener);

            addSlabWindow(window);
            return;

        } catch (...) {

        }

        eventListeners.push_back(listener);
    }

    void SystemWindow::addSlabWindow(Pointer<Window> window) {
        slab_windows.push_back(window);
    }


} // Slab::Core