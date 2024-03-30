//
// Created by joao on 24/09/23.
//

#include "ColorBarArtist.h"
#include "Utils/Resources.h"

#include "Graphics/Graph/Graph.h"
#include "Graphics/Graph/StylesManager.h"

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
    {
        setLocation(loc);
    }

    void OpenGL::ColorBarArtist::draw(const Graph2D &graph) {
        if( texture == nullptr ) return;

        auto style =  Math::StylesManager::GetCurrent();
        auto &writer = style->labelsWriter;

        auto n=5;
        auto dx = rect.width();
        auto dy = rect.height();
        for(int i=0; i<n; ++i){
            auto val = float(i)/float(n-1);
            auto yNorm = float(i)/float(n)- ui;

            writer->write(ToStr(1-val), {(float)rect.xMax+dx*0.1, (float)rect.yMax-yNorm*dy}, style->graphTitleColor);
        }

        auto vp = graph.getViewport();

        texture->bind();

        shader.setUniform("colormap", texture->getTextureUnit());
        shader.setUniform("vpWidth", vp.width());
        shader.setUniform("vpHeight", vp.height());

        vertexBuffer.render(GL_TRIANGLES);
    }

    void ColorBarArtist::setTexture(std::shared_ptr<Texture> tex) { texture = tex; }

    void ColorBarArtist::setLocation(RectI loc) {
        rect = loc;

        fix xMin = loc.xMin;
        fix xMax = loc.xMax;
        fix yMin = loc.yMin;
        fix yMax = loc.yMax;

        vertexBuffer.clear();
        GLuint indices[6] = {0, 1, 2, 0, 2, 3};
        ColorBarVertex vertices[4] = {
                {xMin, yMin,   ui},
                {xMax, yMin,   ui},
                {xMax, yMax,   uf},
                {xMin, yMax,   uf}};

        vertexBuffer.pushBack(vertices, 4, indices, 6);
    }

} // OpenGL