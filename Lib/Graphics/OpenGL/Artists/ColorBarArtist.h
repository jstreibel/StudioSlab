//
// Created by joao on 24/09/23.
//

#ifndef STUDIOSLAB_COLORBARARTIST_H
#define STUDIOSLAB_COLORBARARTIST_H

#include "Overlay.h"

namespace Graphics::OpenGL {

    class ColorBarArtist : public Artist {
        VertexBuffer vertexBuffer;
        Shader shader;
        std::shared_ptr<Texture> texture;
        RectI rect;

    public:
        explicit ColorBarArtist(RectI loc);

        void setLocation(RectI loc);
        void draw(const Graph2D &) override;

        void setTexture(std::shared_ptr<Texture>);
    };

} // OpenGL

#endif //STUDIOSLAB_COLORBARARTIST_H
