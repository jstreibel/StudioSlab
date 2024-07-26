//
// Created by joao on 7/25/24.
//

#ifndef STUDIOSLAB_COLORMAP1DPAINTER_H
#define STUDIOSLAB_COLORMAP1DPAINTER_H

#include "Painter.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

#include "Graphics/Styles/ColorMap.h"

#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Graphics/OpenGL/Texture1D_Color.h"
#include "Graphics/OpenGL/Artists/ColorBarArtist.h"

#include "Graphics/Types2D.h"


namespace Slab::Graphics {

    class Colormap1DPainter : public Painter {
        Pointer<OpenGL::Texture2D_Real> field_data;

        Pointer<OpenGL::Texture1D_Color> cmap_texture;
        Pointer<ColorMap> colormap = ColorMaps["BrBG"]->clone();
        Pointer<OpenGL::ColorBarArtist> colorbarArtist = New<OpenGL::ColorBarArtist>();
        void setColormapTexture(Pointer<OpenGL::Texture1D_Color>);

        bool dirty_minmax = true;
        Real field_min = 0.0;
        Real field_max = 1.0;

        // Vertex
        glm::mat3x3 transform = {1,0,0,
                                 0,1,0,
                                 0,0,1};
        void setTransform(glm::mat3x3);

        // Fragment
        float kappa = 1; //  contrast
        float saturation_value = 1.1;
        float eps_offset = 0.0f;
        bool symmetric_maxmin = true;

        void setKappa(Real);
        void setSaturation(Real);
        void setEpsilon(Real);
        void setSymmetricMaxMin(bool);

        void updateColorbar();

    public:
        Colormap1DPainter();

        void setFieldDataTexture(Pointer<OpenGL::Texture2D_Real>);
        void setRegion(Rect<Real>);

        void setColorMap(const Pointer<ColorMap> &colorMap);

        virtual void labelUpdateEvent(const Str&);

        void drawGUI() override;
        void use() const override;

        bool dirtyMinMax() const;
        void setMinMax(Real, Real);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_COLORMAP1DPAINTER_H
