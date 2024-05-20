//
// Created by joao on 11/10/23.
//

#ifndef STUDIOSLAB_TESTACTOR_H
#define STUDIOSLAB_TESTACTOR_H

#include "Actor.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Texture2D_Real.h"

namespace Graphics {

    class TestActor : public Actor {
        OpenGL::Shader program;
        OpenGL::VertexBuffer vertexBuffer;
        OpenGL::Texture2D_Real texture;

    public:
        TestActor();

        void draw(const Graph3D &graph3D) override;

        void setAmbientLight(Color color);
    };

} // Graphics

#endif //STUDIOSLAB_TESTACTOR_H
