//
// Created by joao on 4/10/23.
//

#include "Graph3DTests.h"
#include "Graphics/Graph3D/Graph3D.h"

namespace Tests {
    Graph3DTests::Graph3DTests() {
        addWindow(std::make_shared<Graphics::Graph3D>());
    }

    void Graph3DTests::draw() { WindowRow::draw(); }
} // Tests