//
// Created by joao on 24/09/23.
//

#include "ColorBarArtist.h"

#include <utility>
#include "Utils/Resources.h"

#include "Graphics/Graph/Graph.h"
#include "Graphics/Graph/StylesManager.h"
#include "Graphics/OpenGL/Texture1D_Color.h"

namespace Graphics::OpenGL {

    struct ColorBarVertex {
        int x, y;
        float u;

    };

    const auto ui = -0.1f;
    const auto uf = +1.1f;

    ColorBarArtist::ColorBarArtist(RectI loc)
            : vertexBuffer("inPosition:2f,inTexCoord:1f")
            , shader(Resources::ShadersFolder + "ColorBar.vert",
                     Resources::ShadersFolder + "ColorBar.frag")
            , inverseScalingFunction([](Real x){ return x; })
    {
        setLocation(loc);
    }

    void OpenGL::ColorBarArtist::draw(const Graph2D &graph) {
        if( texture == nullptr ) return;

        if(1) return;

        auto style =  Math::StylesManager::GetCurrent();
        auto &writer = style->labelsWriter;

        OpenGL::Shader::remove();
        if(0) {
            // GAMBIARRAS

            auto slack = 15.0;

            auto xMin = rect.xMin - slack;
            auto xMax = rect.xMax + writer->getFontHeightInPixels()*5;
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

            glColor3f(1.0,1.0,1.0);
            glBegin(GL_QUADS);
            glVertex2f(xMin, yMin);
            glVertex2f(xMin, yMax);
            glVertex2f(xMax, yMax);
            glVertex2f(xMax, yMin);
            glEnd();

            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex2f(xMin, yMin);
            glVertex2f(xMin, yMax);
            glVertex2f(xMax, yMax);
            glVertex2f(xMax, yMin);
            glEnd();

            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
        }

        auto n=5;
        auto dx = rect.width();
        auto dy = rect.height();
        for(int i=0; i<n; ++i){
            auto s = (Real(i)/Real(n-1));
            auto yNorm = float(i)/float(n)- ui;

            auto val = inverseScalingFunction(s);

            writer->write(ToStr(val),
                          {(float)rect.xMax+dx*0.1, (float)rect.yMax-yNorm*dy}, style->graphTitleColor);
        }

        auto vp = graph.getViewport();

        texture->bind();

        shader.setUniform("colormap", texture->getTextureUnit());
        shader.setUniform("vpWidth", vp.width());
        shader.setUniform("vpHeight", vp.height());


        vertexBuffer.render(GL_TRIANGLES);
    }

    void ColorBarArtist::updateTexture(int samples) {
        if(!textureDirty) return;

        if(texture==nullptr || texture->getSize()!=samples) {
            texture = std::make_shared<::Graphics::OpenGL::Texture1D_Color>(samples, GL_TEXTURE1);
            texture->setWrap(::Graphics::OpenGL::ClampToEdge);
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

    void ColorBarArtist::setColorMap(const Styles::ColorMap& map) {
        this->colorMap = map;

        textureDirty = true;
    }

    auto ColorBarArtist::getTexture() -> CMapTexturePtr {
        updateTexture();

        return texture;
    }

    void ColorBarArtist::setInverseScalingFunction(std::function<Real(Real)> func) {
        inverseScalingFunction = std::move(func);
    }


} // OpenGL