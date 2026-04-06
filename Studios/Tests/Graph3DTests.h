//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_GRAPH3DTESTS_H
#define STUDIOSLAB_GRAPH3DTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"

namespace Tests {

    class FGraph3DTests : public Slab::Graphics::FWindowRow {
    public:
        FGraph3DTests();

        void ImmediateDraw(const Slab::Graphics::FPlatformWindow&) override;
    };

    using Graph3DTests [[deprecated("Use FGraph3DTests")]] = FGraph3DTests;

} // Tests

#endif //STUDIOSLAB_GRAPH3DTESTS_H
