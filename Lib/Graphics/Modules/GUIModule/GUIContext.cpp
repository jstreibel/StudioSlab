//
// Created by joao on 11/3/24.
//

#include "GUIContext.h"

namespace Slab::Graphics {

    GUIContext::GUIContext(FOwnerSystemWindow Parent)
    : FSystemWindowEventListener(Parent) { }

    void GUIContext::AddDrawCall(const FDrawCall& DrawCall) {
        DrawCalls.emplace_back(DrawCall);
    }

    void GUIContext::FlushDrawCalls() {
        for(auto &ExternalDraw : DrawCalls) ExternalDraw();

        DrawCalls.clear();
    }

} // Slab::Graphics