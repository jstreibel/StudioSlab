//
// Created by joao on 4/10/23.
//

#include "Field2DActor.h"

#include "Utils/Resources.h"

namespace Graphics {

    struct Field2DVertex{
        float nx, ny;
        float x, y, z;
        float s, t;
    };

    Field2DActor::Field2DActor()
    : program(Resources::ShadersFolder + "FieldShading.vert", Resources::ShadersFolder + "FieldShading.frag")
    , vertices("normal:3f,position:3f,texcoord:2f")
    {

    }

    void Field2DActor::draw(const Graph3D &graph3D) {
        program.use();

        vertices.render(GL_TRIANGLES);
    }
} // Graphics