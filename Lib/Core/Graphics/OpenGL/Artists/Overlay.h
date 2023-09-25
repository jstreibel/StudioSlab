//
// Created by joao on 24/09/23.
//

#ifndef STUDIOSLAB_OVERLAY_H
#define STUDIOSLAB_OVERLAY_H


#include "Core/Graphics/Artists/Artist.h"
#include "Core/Graphics/OpenGL/VertexBuffer.h"
#include "Core/Graphics/OpenGL/Shader.h"
#include "Core/Graphics/Artists/Rect.h"

namespace OpenGL {

    class Overlay : public Artist {
        VertexBuffer vertexBuffer;
        Shader shader;

    public:
        explicit Overlay(RectI loc);

        void draw(const Window *window) override;
    };

}

#endif //STUDIOSLAB_OVERLAY_H
