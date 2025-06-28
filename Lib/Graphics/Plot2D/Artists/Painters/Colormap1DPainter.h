//
// Created by joao on 7/25/24.
//

#ifndef STUDIOSLAB_COLORMAP1DPAINTER_H
#define STUDIOSLAB_COLORMAP1DPAINTER_H

#include "RealValued2DPainter.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

#include "Graphics/Styles/ColorMap.h"

#include "Graphics/OpenGL/Texture1D_Color.h"
#include "Graphics/OpenGL/Artists/ColorBarArtist.h"

#include "Graphics/Types2D.h"


namespace Slab::Graphics {

    class Colormap1DPainter final : public R2toRPainter {
        Pointer<OpenGL::Texture1D_Color> cmap_texture;
        Pointer<ColorMap> colormap = ColorMaps["BrBG"]->clone();
        Pointer<OpenGL::ColorBarArtist> colorbarArtist = New<OpenGL::ColorBarArtist>();
        void setColormapTexture(Pointer<OpenGL::Texture1D_Color>);

        bool dirty_minmax = true;
        DevFloat field_min = 0.0;
        DevFloat field_max = 1.0;

        // Fragment
        float kappa = 1; //  contrast
        float saturation_value = 1.1;
        float eps_offset = 0.0f;
        bool symmetric_maxmin = true;

        void setKappa(DevFloat) const;
        void setSaturation(DevFloat);
        void setEpsilon(DevFloat);
        void setSymmetricMaxMin(bool);

        void updateColorbar();

    public:
        Colormap1DPainter();

        void setColorMap(const Pointer<ColorMap> &colorMap);

        auto getColorBarArtist() -> Pointer<OpenGL::ColorBarArtist>;

        void labelUpdateEvent(const Str&) override;

        void drawGUI() override;
        void use() const override;

        bool dirtyMinMax() const override;
        void setMinMax(DevFloat, DevFloat) override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_COLORMAP1DPAINTER_H
