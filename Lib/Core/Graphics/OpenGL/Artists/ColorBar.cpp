//
// Created by joao on 24/09/23.
//

#include "ColorBar.h"
#include "Utils/Resources.h"

#include "Core/Graphics/Window/Window.h"

namespace OpenGL {

    struct ColorBarVertex {
        int x, y;
        float u;

    };

    ColorBar::ColorBar(RectI loc)
            : vertexBuffer("inPosition:2f,inTexCoord:1f")
            , shader(Resources::ShadersFolder + "ColorBar.vert",
                     Resources::ShadersFolder + "ColorBar.frag")
    {
        setLocation(loc);
    }

    void OpenGL::ColorBar::draw(const Window *window) {
        if( texture == nullptr ) return;

        auto vp = window->getViewport();

        texture->bind();

        shader.setUniform("colormap", texture->getTextureUnit());
        shader.setUniform("vpWidth", vp.width());
        shader.setUniform("vpHeight", vp.height());

        vertexBuffer.render(GL_TRIANGLES);
    }

    void ColorBar::setTexture(std::shared_ptr<Texture> tex) { texture = tex; }

    void ColorBar::setLocation(RectI loc) {
        fix xMin = loc.xMin;
        fix xMax = loc.xMax;
        fix yMin = loc.yMin;
        fix yMax = loc.yMax;

        auto ui = -0.1f;
        auto uf = +1.1f;

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