//
// Created by joao on 7/26/24.
//

#include "HeightmapShadingPainter.h"

#include "3rdParty/ImGui.h"
#include "Core/Tools/Resources.h"

namespace Slab::Graphics {

    HeightmapShadingPainter::HeightmapShadingPainter()
    : R2toRPainter(Core::Resources::ShadersFolder+"FlatField_relief.frag") {
        setLightZenith(light_zenith_rad);
        setLightAzimuth(light_azimuth_rad);
        setScale(scale);
        setTexelSize(dr);
    }

    void HeightmapShadingPainter::setFieldDataTexture(Pointer<OpenGL::Texture2D_Real> pointer) {
        auto dx = 1./pointer->getWidth();
        auto dy = 1./pointer->getHeight();

        setTexelSize({dx, dy});

        R2toRPainter::setFieldDataTexture(pointer);
    }

    void HeightmapShadingPainter::setScale(Real s) {
        scale = (float)s;
        setUniform("scale", scale);
    }

    void HeightmapShadingPainter::setTexelSize(const Math::Real2D& size) {
        dr = size;
        setUniform("dr", dr);
    }

    void HeightmapShadingPainter::drawGUI() {
        if(ImGui::SliderFloat("scale##", &scale, 1e-1f, 1e2f))
            setScale(scale);
        if(ImGui::SliderAngle("light zenith##", &light_zenith_rad, 0.0f, +90.0f))
            setLightZenith(light_zenith_rad);
        if(ImGui::SliderAngle("light azimuth##", &light_azimuth_rad, 0.0f, +360.0f))
            setLightAzimuth(light_azimuth_rad);

        Painter::drawGUI();
    }

    void HeightmapShadingPainter::setLightZenith(Real zenith) {
        light_zenith_rad = (float)zenith;
        setUniform("light_zenith", light_zenith_rad);
    }

    void HeightmapShadingPainter::setLightAzimuth(Real azimuth) {
        light_azimuth_rad = (float)azimuth;
        setUniform("light_azimuth", light_azimuth_rad);
    }

} // Slab::Graphics