//
// Created by joao on 18/08/23.
//

#include "Texture.h"
#include "Base/Tools/Log.h"

#define R 0
#define G 1
#define B 2
#define A 3

namespace OpenGL {
    Texture::Texture(GLsizei width, GLsizei height) : w(width), h(height) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 0.f, 1.f, 0.f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        data = (ByteData)malloc(w*h*4);

        fix sizeMB = w*h*4/(1024*1024.);
        Log::Critical() << "OpenGL::Texture is allocating " << sizeMB << "MB of GPU texture data to upload history to." << Log::Flush;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        Log::Success() << "OpenGL::Texture allocated " << sizeMB << "MB of GPU texture data." << Log::Flush;
    }

    void Texture::setColor(int i, int j, Styles::Color color) {
        fix index = i*4 + j*w*4;
        ByteData texel = &data[index];

        texel[R] = (Byte)(255*color.r);
        texel[G] = (Byte)(255*color.g);
        texel[B] = (Byte)(255*color.b);
        texel[A] = (Byte)(255*color.a);
    }

    void Texture::upload(PosInt row0, Count nRows) {
        glBindTexture(GL_TEXTURE_2D, texture);

        if(row0==0 && nRows==0)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row0, w, nRows, GL_RGBA, GL_UNSIGNED_BYTE, &data[row0 * w * 4]);
        }

    }

    void Texture::setData(ByteData newData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, newData);
    }

    GLsizei Texture::getWidth() const {
        return w;
    }

    GLsizei Texture::getHeight() const {
        return h;
    }


} // OpenGL