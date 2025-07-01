//
// Created by joao on 7/9/24.
//

#include "WindowProxy.h"

#include <utility>

namespace Slab::Graphics {
    WindowProxy::WindowProxy(FSlabWindowConfig ConfigArg) : FSlabWindow(std::move(ConfigArg)) {

    }

    void WindowProxy::setWindow(Pointer<FSlabWindow> win) {
        window = std::move(win);

        window->NotifyReshape(GetWidth(), GetHeight());

        AddResponder(window);
    }

} // Slab::Graphics