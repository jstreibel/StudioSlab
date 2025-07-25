//
// Created by joao on 7/26/24.
//

#ifndef STUDIOSLAB_REALVALUED2DPAINTER_H
#define STUDIOSLAB_REALVALUED2DPAINTER_H

#include "Painter.h"
#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Graphics/Types2D.h"

namespace Slab::Graphics {

    class R2toRPainter : public Painter {
        TPointer<OpenGL::Texture2D_Real> field_data;

        // Vertex
        glm::mat3x3 transform = {1,0,0,
                                 0,1,0,
                                 0,0,1};
        void setTransform(glm::mat3x3);

    protected:
        explicit R2toRPainter(const Str& frag_shader_source_file);
    public:
        R2toRPainter() = delete;

        void Use() const override;

        virtual void
        setFieldDataTexture(TPointer<OpenGL::Texture2D_Real>);

        virtual void setRegion(RectR);
        virtual bool dirtyMinMax() const;
        virtual void setMinMax(DevFloat, DevFloat);
        virtual void labelUpdateEvent(const Str&);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_REALVALUED2DPAINTER_H
