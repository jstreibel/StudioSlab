//
// Created by joao on 4/10/23.
//

#include "Graph3D.h"
#include "Field2DActor.h"

namespace Graphics {
    Graph3D::Graph3D() {
        actors.emplace_back(std::make_shared<Field2DActor>());
    }

    void Graph3D::draw() {
        Window::draw();
    }


} // Graphics