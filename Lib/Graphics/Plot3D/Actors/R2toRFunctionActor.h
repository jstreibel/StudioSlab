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

    class FR2toRFunctionActor : public FActor {
        R2toR::FNumericFunction_constptr func;

        struct GridMetadata {
            static auto FromNumericFunction(const R2toR::FNumericFunction_constptr&) -> GridMetadata;
            auto generateXYPlane(OpenGL::FVertexBuffer &buffer) const -> void;
            int gridN = 64;
            int gridM = 64;
            float xMinSpace = -8.f;
            float yMinSpace = -8.f;
            float wSpace = 2.f*(float)fabsf(xMinSpace);
            float hSpace = 2.f*(float)fabsf(yMinSpace);
        } gridMetadata;

        OpenGL::FShader program;
        OpenGL::FVertexBuffer vertexBuffer;
        OpenGL::Texture2D_Real texture;

        void rebuildTextureData();

    public:
        explicit FR2toRFunctionActor(R2toR::FNumericFunction_constptr function);

        void draw(const FScene3DWindow &graph3D) override;

        bool hasGUI() override;

        void drawGUI() override;

        void setAmbientLight(FColor color);
        void setGridSubdivs(int n);
    };

    DefinePointers(FR2toRFunctionActor)

    using R2toRFunctionActor [[deprecated("Use FR2toRFunctionActor")]] = FR2toRFunctionActor;
    using R2toRFunctionActor_ptr [[deprecated("Use FR2toRFunctionActor_ptr")]] = FR2toRFunctionActor_ptr;
    using R2toRFunctionActor_constptr [[deprecated("Use FR2toRFunctionActor_constptr")]] = FR2toRFunctionActor_constptr;
    using R2toRFunctionActor_ref [[deprecated("Use FR2toRFunctionActor_ref")]] = FR2toRFunctionActor_ref;
    using R2toRFunctionActor_constref [[deprecated("Use FR2toRFunctionActor_constref")]] = FR2toRFunctionActor_constref;

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONACTOR_H
