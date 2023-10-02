//
// Created by joao on 24/09/23.
//

#ifndef STUDIOSLAB_COLORBAR_H
#define STUDIOSLAB_COLORBAR_H

#include "Overlay.h"

namespace Graphics::OpenGL {

    class ColorBar : public Artist {
        VertexBuffer vertexBuffer;
        Shader shader;
        std::shared_ptr<Texture> texture;

    public:
        explicit ColorBar(RectI loc);

        void setLocation(RectI loc);
        void draw(const Graph2D &) override;

        void setTexture(std::shared_ptr<Texture>);
    };

} // OpenGL

#endif //STUDIOSLAB_COLORBAR_H
