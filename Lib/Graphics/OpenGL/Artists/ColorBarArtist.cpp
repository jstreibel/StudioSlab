//
// Created by joao on 24/09/23.
//

#include "ColorBarArtist.h"

#include <utility>
#include "Core/Tools/Resources.h"

#include "Graphics/Graph/PlottingWindow.h"
#include "Graphics/Graph/PlotThemeManager.h"

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
            , inverseScalingFunction([](Real x){ return x; })
    {
        setLocation(loc);
    }

    bool OpenGL::ColorBarArtist::draw(const PlottingWindow &graph) {
        if( texture == nullptr ) return true;

        auto style =  PlotThemeManager::GetCurrent();
        auto &writer = style->labelsWriter;

        OpenGL::Shader::remove();
        if(1) {
            // GAMBIARRAS

            auto slack = 15.0;

            auto xMin = rect.xMin - slack;
            auto xMax = rect.xMax + writer->getFontHeightInPixels()*6;
            auto yMin = rect.yMin + slack;
            auto yMax = rect.yMax - slack;

            xMin = 2.f*(xMin/graph.getViewport().width()-.5f);
            xMax = 2.f*(xMax/graph.getViewport().width()-.5f);
            yMin = 2.f*(yMin/graph.getViewport().height()-.5f);
            yMax = 2.f*(yMax/graph.getViewport().height()-.5f);

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

        auto n=9;
        auto dx = rect.width();
        auto dy = rect.height();
        auto Δu = uf-ui;
        // writer->getFontHeightInPixels();

        for(int i=0; i<n; ++i){
            auto t = (Real(i)/Real(n-1));
            auto yMeasure = t/Δu-ui; //(float(i)/float(n)) - ui;

            Real phi;
            if(params.mode == ColorBarMode::AllFieldValues)
                phi = t*(params.phi_max-params.phi_min)+params.phi_min;
            else if(params.mode == ColorBarMode::ValuesInSatRangeOnly) {
                if (params.symmetric)
                    phi = params.phi_sat * (2 * t - 1);
                else
                    phi = params.phi_sat * t;
            }

            writer->write(ToStr(phi),
                          {(float)rect.xMax+dx*0.1, (float)rect.yMax-yMeasure*dy}, style->graphTitleColor);
        }

        auto vp = graph.getViewport();

        texture->bind();

        shader.setUniform("colormap", texture->getTextureUnit());
        shader.setUniform("vpWidth", vp.width());
        shader.setUniform("vpHeight", vp.height());

        vertexBuffer.render(GL_TRIANGLES);

        return true;
    }

    void ColorBarArtist::updateTexture(int samples) {
        if(!textureDirty) return;

        if(texture==nullptr || texture->getSize()!=samples) {
            texture = std::make_shared<OpenGL::Texture1D_Color>(samples, GL_TEXTURE1);
            texture->setWrap(OpenGL::ClampToEdge);
        }

        for(auto i=0; i<samples; ++i){
            fix s = (Real)(i-1)/(Real)(samples-2);
            fix color = colorMap.mapValueToColor(s);
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

    void ColorBarArtist::setColorMap(const ColorMap& map) {
        this->colorMap = map;

        textureDirty = true;
    }

    auto ColorBarArtist::getTexture() -> CMapTexturePtr {
        fix samples = 4*4096;
        updateTexture(samples);

        return texture;
    }

    void ColorBarArtist::setScalingFunction(std::function<Real(Real)> func) {
        scalingFunction = std::move(func);
    }

    void ColorBarArtist::setInverseScalingFunction(std::function<Real(Real)> func) {
        inverseScalingFunction = std::move(func);
    }

    void ColorBarArtist::setKappa(Real kappa) {
        shader.setUniform("kappa", (float)kappa);

        params.kappa = kappa;
    }

    void ColorBarArtist::setPhiSaturation(Real phiSat) {
        shader.setUniform("phi_sat", (float)phiSat);

        params.phi_sat = phiSat;
    }

    void ColorBarArtist::setSymmetric(bool symmetric) {
        shader.setUniform("symmetric", symmetric);

        params.symmetric = symmetric;
    }

    void ColorBarArtist::setPhiMax(Real phiMax) {
        shader.setUniform("phi_max", (float)phiMax);

        params.phi_max = phiMax;
    }

    void ColorBarArtist::setPhiMin(Real phiMin) {
        shader.setUniform("phi_min", (float)phiMin);

        params.phi_min = phiMin;
    }

    void ColorBarArtist::setMode(ColorBarMode mode) {
        shader.setUniform("mode", (int)mode);

        params.mode = mode;
    }




} // OpenGL