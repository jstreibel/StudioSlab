//
// Created by joao on 11/3/24.
//

#include "GUIContext.h"

namespace Slab::Graphics {

    GUIContext::GUIContext(ParentSystemWindow parent)
    : FSystemWindowEventListener(parent) { }

    void GUIContext::AddDrawCall(const DrawCall& draw_call) {
        draw_calls.emplace_back(draw_call);
    }

    void GUIContext::FlushDrawCalls() {
        for(auto &external_draw : draw_calls) external_draw();

        draw_calls.clear();
    }

} // Slab::Graphics