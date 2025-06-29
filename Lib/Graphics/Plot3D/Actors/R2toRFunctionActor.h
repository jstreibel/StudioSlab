//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_R2TORFUNCTIONACTOR_H
#define STUDIOSLAB_R2TORFUNCTIONACTOR_H

#include "Graphics/Plot3D/Actor.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Slab::Graphics {

    using namespace Math;

    class R2toRFunctionActor : public Actor {
        R2toR::FNumericFunction_constptr func;

        struct GridMetadata {
            static auto FromNumericFunction(const R2toR::FNumericFunction_constptr&) -> GridMetadata;
            auto generateXYPlane(OpenGL::VertexBuffer &buffer) const -> void;
            int gridN = 64;
            int gridM = 64;
            float xMinSpace = -8.f;
            float yMinSpace = -8.f;
            float wSpace = 2.f*(float)fabsf(xMinSpace);
            float hSpace = 2.f*(float)fabsf(yMinSpace);
        } gridMetadata;

        OpenGL::Shader program;
        OpenGL::VertexBuffer vertexBuffer;
        OpenGL::Texture2D_Real texture;

        void rebuildTextureData();

    public:
        explicit R2toRFunctionActor(R2toR::FNumericFunction_constptr function);

        void draw(const Scene3DWindow &graph3D) override;

        bool hasGUI() override;

        void drawGUI() override;

        void setAmbientLight(Color color);
        void setGridSubdivs(int n);
    };

    DefinePointers(R2toRFunctionActor)

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONACTOR_H
