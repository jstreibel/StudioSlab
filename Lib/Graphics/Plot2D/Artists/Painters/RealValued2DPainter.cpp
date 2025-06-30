//
// Created by joao on 7/26/24.
//

#include "RealValued2DPainter.h"
#include "Core/Tools/Resources.h"

#include <utility>

namespace Slab::Graphics {

    R2toRPainter::R2toRPainter(const Str& frag_shader_source_file)
    : Painter(Core::Resources::ShadersFolder+"FlatField.vert",
              frag_shader_source_file) {

    }

    bool R2toRPainter::dirtyMinMax() const { return false; }

    void R2toRPainter::setMinMax(DevFloat, DevFloat) { }

    void R2toRPainter::labelUpdateEvent(const Str &) { }

    void R2toRPainter::Use() const {
        if(field_data) field_data->Bind();

        Painter::Use();
    }

    void R2toRPainter::setRegion(RectR  graphRect) {
        fix x = graphRect.xMin, y = graphRect.yMin, w = graphRect.GetWidth(), h = graphRect.GetHeight();

        fix xScale = 2.f / w;
        fix xTranslate = -1.0f - 2.0f * x / w;
        fix yScale = 2.f / h;
        fix yTranslate = -1.0f - 2.0f * y / h;

        glm::mat3x3 transf = {
                xScale, 0.0f, 0.0f,
                0.0f, yScale, 0.0f,
                xTranslate, yTranslate, 1.0f
        };

        setTransform(transf);
    }

    void R2toRPainter::setFieldDataTexture(Pointer<OpenGL::Texture2D_Real> data) {
        field_data = std::move(data);
        field_data->Bind();
        SetUniform("field_data", field_data->getTextureUnit());
    }

    void R2toRPainter::setTransform(glm::mat3x3 transf) {
        this->transform = transf;
        SetUniform("transformMatrix", transform);
    }


} // Slab::Graphics