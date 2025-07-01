//
// Created by joao on 24/09/23.
//

#ifndef STUDIOSLAB_OVERLAY_H
#define STUDIOSLAB_OVERLAY_H


#include "Graphics/Plot2D/Artists/Artist.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Shader.h"

namespace Slab::Graphics::OpenGL {

    class Overlay : public FArtist {
        FVertexBuffer vertexBuffer;
        FShader shader;

    public:
        explicit Overlay(RectI loc);

        bool Draw(const FPlot2DWindow &) override;
    };

}

#endif //STUDIOSLAB_OVERLAY_H
