//
// Created by joao on 11/1/24.
//

#ifndef STUDIOSLAB_IMAGELOAD_H
#define STUDIOSLAB_IMAGELOAD_H

#include "Graphics/OpenGL/Texture2D_Color.h"

namespace Slab::Graphics::Image {
    Pointer<OpenGL::Texture2D_Color> LoadTexture(const Str &image_file);
}

#endif //STUDIOSLAB_IMAGELOAD_H
