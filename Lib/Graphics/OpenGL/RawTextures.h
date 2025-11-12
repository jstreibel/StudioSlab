//
// Created by joao on 10/23/24.
//

#ifndef STUDIOSLAB_IMAGES_H
#define STUDIOSLAB_IMAGES_H

#include "3rdParty/stb_image.h"
#include "Utils/String.h"
#include "Utils/Arrays.h"

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/Utils/ImageLoad.h"

namespace Slab::Graphics {
    using SlabTextureID = void*;

    struct SlabTexture
    {
        GLuint TextureID = 0;
        int    Width     = 0;
        int    Height    = 0;

        static SlabTextureID CreateTexture(Image::StbiImageInfo);

        static void DestroyTexture(SlabTextureID texture);

        static SlabTextureID LoadTextureFromImage(const Str& path);

        static Vector<SlabTexture>::iterator FindTexture(SlabTextureID texture);

        static int GetTextureWidth(SlabTextureID texture);

        static int GetTextureHeight(SlabTextureID texture);
    };
}

#endif //STUDIOSLAB_IMAGES_H
