//
// Created by joao on 7/9/24.
//

#include "WindowProxy.h"

#include <utility>

namespace Slab::Graphics {
    WindowProxy::WindowProxy() : SlabWindow() {

    }

    void WindowProxy::setWindow(Pointer<SlabWindow> win) {
        window = std::move(win);

        window->notifyReshape(GetWidth(), GetHeight());

        addResponder(window);
    }

} // Slab::Graphics