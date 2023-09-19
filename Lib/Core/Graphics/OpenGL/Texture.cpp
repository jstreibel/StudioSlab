//
// Created by joao on 19/09/23.
//

#include "Texture.h"

namespace OpenGL {
    Texture::Texture() {
        glGenTextures(1, &texture);
    }
} // OpenGL