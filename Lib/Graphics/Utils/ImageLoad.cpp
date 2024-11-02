//
// Created by joao on 11/1/24.
//

#include <stb_image.h>
#include "ImageLoad.h"

namespace Slab::Graphics::Image {

    Pointer<OpenGL::Texture2D_Color> LoadTexture(const Str &path) {
        NOT_IMPLEMENTED

        int width = 0, height = 0, component = 0;
        if (auto data = stbi_load(path.c_str(), &width, &height, &component, 4))
        {
            // auto texture = CreateTexture(data, width, height);
            // stbi_image_free(data);
            // return texture;
        }
        else
            return nullptr;

        return Slab::Pointer<OpenGL::Texture2D_Color>();
    }
}