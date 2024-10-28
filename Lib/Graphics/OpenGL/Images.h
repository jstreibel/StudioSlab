//
// Created by joao on 10/23/24.
//

#ifndef STUDIOSLAB_IMAGES_H
#define STUDIOSLAB_IMAGES_H

#include "Utils/String.h"
#include "Utils/Arrays.h"

#include "Graphics/OpenGL/OpenGL.h"

namespace Slab::Graphics {
    using SlabTextureID= void*;

    struct SlabTexture
    {
        GLuint TextureID = 0;
        int    Width     = 0;
        int    Height    = 0;
    };

    SlabTextureID CreateTexture(const void* data, int width, int height);

    void DestroyTexture(SlabTextureID texture);

    SlabTextureID LoadTexture(const Str& path);

    Vector<SlabTexture>::iterator FindTexture(SlabTextureID texture);

    int GetTextureWidth(SlabTextureID texture);

    int GetTextureHeight(SlabTextureID texture);
}

#endif //STUDIOSLAB_IMAGES_H
