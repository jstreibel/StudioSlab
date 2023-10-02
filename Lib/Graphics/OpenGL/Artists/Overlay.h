//
// Created by joao on 24/09/23.
//

#ifndef STUDIOSLAB_OVERLAY_H
#define STUDIOSLAB_OVERLAY_H


#include "Graphics/Artists/Artist.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Shader.h"

namespace Graphics::OpenGL {

    class Overlay : public Artist {
        VertexBuffer vertexBuffer;
        Shader shader;

    public:
        explicit Overlay(RectI loc);

        void draw(const Graph2D &) override;
    };

}

#endif //STUDIOSLAB_OVERLAY_H
