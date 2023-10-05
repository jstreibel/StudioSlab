//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_GRAPH3DTESTS_H
#define STUDIOSLAB_GRAPH3DTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"

namespace Tests {

    class Graph3DTests : public Graphics::WindowRow {
    public:
        Graph3DTests();

        void draw() override;
    };

} // Tests

#endif //STUDIOSLAB_GRAPH3DTESTS_H
