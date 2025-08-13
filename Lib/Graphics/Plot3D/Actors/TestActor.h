//
// Created by joao on 11/10/23.
//

#ifndef STUDIOSLAB_TESTACTOR_H
#define STUDIOSLAB_TESTACTOR_H

#include "Graphics/Plot3D/Actor.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Texture2D_Real.h"

namespace Slab::Graphics {

    class TestActor : public Actor {
        OpenGL::FShader program;
        OpenGL::FVertexBuffer vertexBuffer;
        OpenGL::Texture2D_Real texture;

    public:
        TestActor();

        void draw(const Scene3DWindow &graph3D) override;

        bool hasGUI() override;

        void drawGUI() override;

        void setAmbientLight(FColor color);
    };

} // Graphics

#endif //STUDIOSLAB_TESTACTOR_H
