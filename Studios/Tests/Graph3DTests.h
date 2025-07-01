//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_GRAPH3DTESTS_H
#define STUDIOSLAB_GRAPH3DTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"

namespace Tests {

    class Graph3DTests : public Slab::Graphics::FWindowRow {
    public:
        Graph3DTests();

        void ImmediateDraw(const Slab::Graphics::FPlatformWindow&) override;
    };

} // Tests

#endif //STUDIOSLAB_GRAPH3DTESTS_H
