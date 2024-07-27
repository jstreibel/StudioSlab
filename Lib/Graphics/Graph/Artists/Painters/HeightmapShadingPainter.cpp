//
// Created by joao on 7/26/24.
//

#include "HeightmapShadingPainter.h"

#include "3rdParty/ImGui.h"
#include "Core/Tools/Resources.h"

namespace Slab::Graphics {

    HeightmapShadingPainter::HeightmapShadingPainter()
    : R2toRPainter(Core::Resources::ShadersFolder+"FlatField_relief.frag") {
        setLightIntensity(light_intensity);
        setLightZenith(light_zenith_rad);
        setLightAzimuth(light_azimuth_rad);
        setSigma(sigma);
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
        if(ImGui::DragFloat("scale##", &scale, scale*1.e-3f, 1e-6f, 1e6f, "%.3e"))
            setScale(scale);
        if(ImGui::SliderAngle("light zenith##", &light_zenith_rad, 0.0f, +90.0f))
            setLightZenith(light_zenith_rad);
        if(ImGui::SliderAngle("light azimuth##", &light_azimuth_rad, 0.0f, +180.0f))
            setLightAzimuth(light_azimuth_rad);
        if(ImGui::SliderFloat("light intensity##", &light_intensity, +1e-1f, +1e+1f))
            setLightIntensity(light_intensity);

        ImGui::SeparatorText("gradient mode");
        ImGui::BeginGroup();
        if(ImGui::RadioButton("symmetric", mode==1)) setMode(1);
        if(ImGui::RadioButton("right",     mode==2)) setMode(2);
        if(ImGui::RadioButton("left",      mode==3)) setMode(3);
        ImGui::EndGroup();
        ImGui::Separator();
        if(ImGui::SliderFloat("σ##", &sigma, 5e-2f, 2.f))
            setSigma(sigma);
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

    void HeightmapShadingPainter::setLightIntensity(Real intensity) {
        light_intensity = (float)intensity;
        setUniform("light_intensity", light_intensity);
    }

    void HeightmapShadingPainter::setMode(int m) {
        mode = m;
        setUniform("mode", mode);
    }

    void HeightmapShadingPainter::setSigma(Real s) {
        sigma = s;
        setUniform("sigma", sigma);
    }

} // Slab::Graphics