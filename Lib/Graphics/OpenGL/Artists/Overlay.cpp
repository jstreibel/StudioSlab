//
// Created by joao on 24/09/23.
//

#include "Overlay.h"
#include "Core/Tools/Resources.h"

#include "Graphics/Graph/PlottingWindow.h"



namespace Slab::Graphics::OpenGL {
    struct OverlayVertex {
        int x, y;
        float r, g, b, a;
        float u, v;

    };

    Overlay::Overlay(RectI loc)
    : vertexBuffer("inPosition:2f,inColor:4f,inTexCoord:2f")
    , shader(Core::Resources::ShadersFolder + "ScreenCoords2D.vert",
             Core::Resources::ShadersFolder + "ScreenCoords2D.frag")
    {
        fix xMin = loc.xMin;
        fix xMax = loc.xMax;
        fix yMin = loc.yMin;
        fix yMax = loc.yMax;

        auto si = 0.0f;
        auto sf = 1.0f;
        auto ti = 0.0f;
        auto tf = 1.0f;

        vertexBuffer.clear();
        GLuint indices[6] = {0, 1, 2, 0, 2, 3};
        OverlayVertex vertices[4] = {
                {xMin, yMin,    1,1,1,1,    si, ti},
                {xMax, yMin,    1,1,1,1,    sf, ti},
                {xMax, yMax,    1,1,1,1,    sf, tf},
                {xMin, yMax,    1,1,1,1,    si, tf}};

        vertexBuffer.pushBack(vertices, 4, indices, 6);
    }

    void OpenGL::Overlay::draw(const PlottingWindow &graph2D) {
        auto vp = graph2D.getViewport();

        shader.setUniform("vpWidth", vp.width());
        shader.setUniform("vpHeight", vp.height());

        vertexBuffer.render(GL_TRIANGLES);
    }

}