//
// Created by joao on 7/26/24.
//

#ifndef STUDIOSLAB_HEIGHTMAPSHADINGPAINTER_H
#define STUDIOSLAB_HEIGHTMAPSHADINGPAINTER_H

#include "RealValued2DPainter.h"
#include "Math/Formalism/Categories.h"

namespace Slab::Graphics {

    class HeightmapShadingPainter : public R2toRPainter {
        float light_zenith_rad = M_PI/4;
        float light_azimuth_rad = M_PI/4;
        float light_intensity = 1.0f;
        int mode = 1;
        float scale = 1;
        float sigma = 1;
        Math::Real2D dr = {1,1};

    public:
        HeightmapShadingPainter();

        void drawGUI() override;

        void setLightZenith(DevFloat rads);
        void setLightAzimuth(DevFloat rads);
        void setLightIntensity(DevFloat);
        void setSigma(DevFloat);
        void setScale(DevFloat);
        void setMode(int);
        void setTexelSize(const Math::Real2D&);

        void setFieldDataTexture(Pointer<OpenGL::Texture2D_Real> pointer) override;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_HEIGHTMAPSHADINGPAINTER_H
