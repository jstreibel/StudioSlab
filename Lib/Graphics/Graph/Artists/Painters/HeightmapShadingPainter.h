//
// Created by joao on 7/26/24.
//

#ifndef STUDIOSLAB_HEIGHTMAPSHADINGPAINTER_H
#define STUDIOSLAB_HEIGHTMAPSHADINGPAINTER_H

#include "RealValued2DPainter.h"
#include "Math/Formalism/Categories.h"

namespace Slab::Graphics {

    class HeightmapShadingPainter : public R2toRPainter {
        float light_zenith_rad = 0.0;
        float light_azimuth_rad = 0.0;
        float scale = 1;
        Math::Real2D dr = {1,1};

    public:
        HeightmapShadingPainter();

        void drawGUI() override;

        void setLightZenith(Real rads);
        void setLightAzimuth(Real rads);
        void setScale(Real);
        void setTexelSize(const Math::Real2D&);

        void setFieldDataTexture(Pointer<OpenGL::Texture2D_Real> pointer) override;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_HEIGHTMAPSHADINGPAINTER_H
