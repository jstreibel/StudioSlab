//
// Created by joao on 11/3/24.
//

#include "GUIContext.h"

namespace Slab::Graphics {

    FGUIContext::FGUIContext()= default;

    void FGUIContext::AddDrawCall(const FDrawCall& DrawCall) {
        DrawCalls.emplace_back(DrawCall);
    }

    void FGUIContext::FlushDrawCalls() {
        for(auto &ExternalDraw : DrawCalls) ExternalDraw();

        DrawCalls.clear();
    }

} // Slab::Graphics