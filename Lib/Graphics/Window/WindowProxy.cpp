//
// Created by joao on 7/9/24.
//

#include "WindowProxy.h"

#include <utility>

namespace Slab::Graphics {
    FWindowProxy::FWindowProxy(FSlabWindowConfig ConfigArg) : FSlabWindow(std::move(ConfigArg)) {

    }

    void FWindowProxy::setWindow(TPointer<FSlabWindow> win) {
        window = std::move(win);

        window->NotifyReshape(GetWidth(), GetHeight());

        AddResponder(window);
    }

} // Slab::Graphics
