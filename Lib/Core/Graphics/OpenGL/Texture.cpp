//
// Created by joao on 19/09/23.
//

#include "Texture.h"

namespace OpenGL {
    Texture::Texture(Target target) : target(target) {
        glGenTextures(1, &texture);
    }

    void Texture::bind() const {
        glBindTexture(target, texture);
    }
} // OpenGL