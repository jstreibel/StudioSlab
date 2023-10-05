//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_FIELD2DACTOR_H
#define STUDIOSLAB_FIELD2DACTOR_H

#include "Actor.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/VertexBuffer.h"

namespace Graphics {

    class Field2DActor : public Actor {
        OpenGL::Shader program;
        OpenGL::VertexBuffer vertices;
    public:
        Field2DActor();

        void draw(const Graph3D &graph3D) override;
    };

} // Graphics

#endif //STUDIOSLAB_FIELD2DACTOR_H
