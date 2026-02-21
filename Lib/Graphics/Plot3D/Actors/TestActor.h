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

    class FTestActor : public FActor {
        OpenGL::FShader program;
        OpenGL::FVertexBuffer vertexBuffer;
        OpenGL::Texture2D_Real texture;

    public:
        FTestActor();

        void draw(const FScene3DWindow &graph3D) override;

        bool hasGUI() override;

        void drawGUI() override;

        void setAmbientLight(FColor color);
    };

    using TestActor [[deprecated("Use FTestActor")]] = FTestActor;

} // Graphics

#endif //STUDIOSLAB_TESTACTOR_H
