//
// Created by joao on 19/09/23.
//

#include "Texture.h"

namespace OpenGL {
    Texture::Texture(Target target, InternalFormat internalFormat1)
    : target(target)
    , internalFormat(internalFormat1) {
        glGenTextures(1, &texture);
    }

    void Texture::bind() const {
        glBindTexture(target, texture);
    }
} // OpenGL