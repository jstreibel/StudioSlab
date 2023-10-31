//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_FIELD2DACTOR_H
#define STUDIOSLAB_FIELD2DACTOR_H

#include "Actor.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"

namespace Graphics {

    class Field2DActor : public Actor {
        std::shared_ptr<R2toR::DiscreteFunction> func;

        struct GridMetadata {
            static auto FromDiscreteFunction(const R2toR::DiscreteFunction&) -> GridMetadata;
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


    public:
        explicit Field2DActor(std::shared_ptr<R2toR::DiscreteFunction> function);

        void draw(const Graph3D &graph3D) override;

        void setAmbientLight(Styles::Color color);
        void setGridSubdivs(int n);
    };

} // Graphics

#endif //STUDIOSLAB_FIELD2DACTOR_H
