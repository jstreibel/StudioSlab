//
// Created by joao on 24/09/23.
//

#ifndef STUDIOSLAB_OVERLAY_H
#define STUDIOSLAB_OVERLAY_H


#include "Graphics/Graph/Artists/Artist.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Shader.h"

namespace Slab::Graphics::OpenGL {

    class Overlay : public Artist {
        VertexBuffer vertexBuffer;
        Shader shader;

    public:
        explicit Overlay(RectI loc);

        bool draw(const PlottingWindow &) override;
    };

}

#endif //STUDIOSLAB_OVERLAY_H
