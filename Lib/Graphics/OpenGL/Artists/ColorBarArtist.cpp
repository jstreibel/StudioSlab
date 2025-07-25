//
// Created by joao on 24/09/23.
//

#include "ColorBarArtist.h"

#include "3rdParty/ImGui.h"

#include <utility>
#include "Core/Tools/Resources.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"

#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

namespace Slab::Graphics::OpenGL {

    struct ColorBarVertex {
        int x, y;
        float u;

    };

    const auto ui = -0.1f;
    const auto uf = +1.1f;

    ColorBarArtist::ColorBarArtist(RectI loc)
    : vertexBuffer("inPosition:2f,inTexCoord:1f")
    , shader(Core::Resources::ShadersFolder + "ColorBar.vert",
             Core::Resources::ShadersFolder + "ColorBar.frag")
    , inverseScalingFunction([](DevFloat x){ return x; })
    {
        setLocation(loc);
    }

    bool OpenGL::ColorBarArtist::Draw(const FPlot2DWindow &graph) {
        updateTexture();
        if( texture == nullptr ) return true;

        const int vpWidth  = graph.GetViewport().GetWidth();
        const int vpHeight = graph.GetViewport().GetHeight();
        const int cbarHeight_pixels = int(cbarHeight_clamp * vpHeight);
        if(autoColorBarTop)
            cbarTop_pixels = (vpHeight - cbarHeight_pixels) / 2;

        auto left_actual = left>=0 ? left : vpWidth+left;
        // TODO it sucks to do this every time.
        setLocation({left_actual,
                     left_actual + cbarWidth_pixels,
                     vpHeight - cbarTop_pixels,
                     vpHeight - cbarTop_pixels - cbarHeight_pixels});

        auto style =  PlotThemeManager::GetCurrent();
        auto &writer = style->LabelsWriter;

        Legacy::PushLegacyMode();
        Legacy::SetupOrthoI(graph.GetViewport());
        // TODO this below also sucks.
        if(1) {
            // GAMBIARRAS

            auto xMin = (float)rect.xMin - (float)general_slack;
            auto xMax = (float)rect.xMax + writer->GetFontHeightInPixels()*(decimal_places) + (float)right_slack;
            auto yMin = (float)rect.yMin + (float)general_slack;
            auto yMax = (float)rect.yMax - (float)general_slack;

            xMin = 2.f*(xMin/(float) graph.GetViewport().GetWidth() - .5f);
            xMax = 2.f*(xMax/(float) graph.GetViewport().GetWidth() - .5f);
            yMin = 2.f*(yMin/(float) graph.GetViewport().GetHeight() - .5f);
            yMax = 2.f*(yMax/(float) graph.GetViewport().GetHeight() - .5f);

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            fix bgColor = PlotThemeManager::GetCurrent()->graphBackground;
            fix fgColor = PlotThemeManager::GetCurrent()->graphNumbersColor;

            glColor4fv(bgColor.asFloat4fv());
            glBegin(GL_QUADS);
            glVertex2d(xMin, yMin);
            glVertex2d(xMin, yMax);
            glVertex2d(xMax, yMax);
            glVertex2d(xMax, yMin);
            glEnd();

            glDisable(GL_LINE_STIPPLE);
            glLineWidth(1);
            glColor4fv(fgColor.asFloat4fv());
            glBegin(GL_LINE_LOOP);
            glVertex2d(xMin, yMin);
            glVertex2d(xMin, yMax);
            glVertex2d(xMax, yMax);
            glVertex2d(xMax, yMin);
            glEnd();

            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
        }
        Legacy::RestoreFromLegacyMode();

        auto dx = rect.GetWidth();
        auto dy = rect.GetHeight();
        auto Δu = uf-ui;
        // writer->getFontHeightInPixels();

        for(int i=0; i<n_annotations; ++i){
            auto t = (DevFloat(i)/DevFloat(n_annotations-1));
            auto yMeasure = t/Δu-ui; //(float(i)/float(n)) - ui;

            DevFloat phi;
            if(params.mode == ColorBarMode::AllFieldValues)
                phi = t*(params.phi_max-params.phi_min)+params.phi_min;
            else if(params.mode == ColorBarMode::ValuesInSatRangeOnly) {
                if (params.symmetric)
                    phi = params.phi_sat * (2 * t - 1);
                else
                    phi = params.phi_sat * t;
            }

            writer->Write(ToStr(phi, decimal_places, scientific_notation),
                          {(float)rect.xMax+dx*0.1, (float)rect.yMax-yMeasure*dy}, style->graphTitleColor);
        }

        auto vp = graph.GetViewport();

        texture->Bind();

        shader.SetUniform("colormap", texture->getTextureUnit());
        shader.SetUniform("vpWidth", vp.GetWidth());
        shader.SetUniform("vpHeight", vp.GetHeight());

        vertexBuffer.Render(GL_TRIANGLES);

        return true;
    }

    void ColorBarArtist::updateTexture() {
        if(!textureDirty) return;

        fix samples = colorMap->getColorCount()*100;

        if(texture==nullptr || texture->getSize() != samples) {
            texture = std::make_shared<OpenGL::Texture1D_Color>(samples, GL_TEXTURE1);
            texture->setWrap(OpenGL::ClampToEdge);
        }

        /*
        int i = 0;
        for(fix color : *colorMap) {
            texture->setColor(i++, color);
        }
         */

        for(auto i=0; i < samples; ++i){
            fix s = (DevFloat)(i-1)/(DevFloat)(samples - 2);
            fix color = colorMap->mapValueToColor(s);
            texture->setColor(i, color);
        }

        texture->upload();

        textureDirty = false;
    }

    void ColorBarArtist::setLocation(RectI loc) {
        rect = loc;

        fix xMin = loc.xMin;
        fix xMax = loc.xMax;
        fix yMin = loc.yMin;
        fix yMax = loc.yMax;

        vertexBuffer.clear();
        GLuint indices[6] = {0, 1, 2, 0, 2, 3};
        ColorBarVertex vertices[4] = {
                {xMin, yMax,   ui},
                {xMax, yMax,   ui},
                {xMax, yMin,   uf},
                {xMin, yMin,   uf}};

        vertexBuffer.pushBack(vertices, 4, indices, 6);
    }

    void ColorBarArtist::DrawGUI() {
        FArtist::DrawGUI();

        ImGui::Checkbox("Auto top", &autoColorBarTop);
        ImGui::NewLine();

        ImGui::SliderInt("x_loc", &left, -2000, 2000);
        ImGui::SliderInt("y_loc", &cbarTop_pixels, 0, 2000);
        ImGui::SliderInt("bar width", &cbarWidth_pixels, 10, 2000);
        ImGui::SliderFloat("bar height %", &cbarHeight_clamp, 1e-1f, 1.f);
        ImGui::SliderInt("general slack", &general_slack, 0, 500);
        ImGui::SliderInt("right slack", &right_slack, 0, 500);

        ImGui::NewLine();
        ImGui::SliderInt("n annotations", &n_annotations, 3, 21);
        ImGui::Checkbox("scientific notation", &scientific_notation);
        ImGui::SliderInt("decimal places", &decimal_places, 0, 15);


    }

    void ColorBarArtist::setColorMap(const TPointer<const ColorMap>& map) {
        this->colorMap = map;

        textureDirty = true;
    }

    auto ColorBarArtist::getTexture() -> CMapTexturePtr {
        updateTexture();

        return texture;
    }

    void ColorBarArtist::setScalingFunction(std::function<DevFloat(DevFloat)> func) {
        scalingFunction = std::move(func);

        textureDirty = true;
    }

    void ColorBarArtist::setInverseScalingFunction(std::function<DevFloat(DevFloat)> func) {
        inverseScalingFunction = std::move(func);

        textureDirty = true;
    }

    void ColorBarArtist::setKappa(DevFloat kappa) {
        shader.SetUniform("kappa", (float)kappa);

        params.kappa = kappa;

        textureDirty = true;
    }

    void ColorBarArtist::setPhiSaturation(DevFloat phiSat) {
        shader.SetUniform("phi_sat", (float)phiSat);

        params.phi_sat = phiSat;

        textureDirty = true;
    }

    void ColorBarArtist::setSymmetric(bool symmetric) {
        shader.SetUniform("symmetric", symmetric);

        params.symmetric = symmetric;

        textureDirty = true;
    }

    void ColorBarArtist::setPhiMax(DevFloat phiMax) {
        shader.SetUniform("phi_max", (float)phiMax);

        params.phi_max = phiMax;

        textureDirty = true;
    }

    void ColorBarArtist::setPhiMin(DevFloat phiMin) {
        shader.SetUniform("phi_min", (float)phiMin);

        params.phi_min = phiMin;

        textureDirty = true;
    }

    void ColorBarArtist::setMode(const ColorBarMode mode) {
        shader.SetUniform("mode", (int)mode);

        params.mode = mode;

        textureDirty = true;
    }

    CountType ColorBarArtist::getSamples() const {
        return colorMap->getColorCount();
    }

    bool ColorBarArtist::HasGUI() { return true; }


} // OpenGL