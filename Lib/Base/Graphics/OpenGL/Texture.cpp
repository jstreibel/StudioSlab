//
// Created by joao on 18/08/23.
//

#include "Texture.h"

#define R 0
#define G 1
#define B 2
#define A 3

namespace OpenGL {
    Texture::Texture(GLsizei width, GLsizei height) : w(width), h(height) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        data = (ByteData*)malloc(w*h*4);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    void Texture::setColor(int i, int j, Styles::Color color) {
        auto *texel = data[i*4 + (j*4)*w];

        texel[R] = (Byte)(255*color.r);
        texel[G] = (Byte)(255*color.g);
        texel[B] = (Byte)(255*color.b);
        texel[A] = (Byte)(255*color.a);
    }

    void Texture::upload() {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    GLsizei Texture::getWidth() const {
        return w;
    }

    GLsizei Texture::getHeight() const {
        return h;
    }
} // OpenGL