//
// Created by joao on 16/09/23.
//

#include "Texture1D.h"
#include "Core/Tools/Log.h"

#define R 0
#define G 1
#define B 2
#define A 3

namespace OpenGL {

    Texture1D::Texture1D(GLsizei size) : size(size) {
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        if(size>maxTextureSize) {
            Log::Error() << "Requested texture size " << size
                         << " too big: max texture size allowed is " << maxTextureSize
                         << Log::Flush;

            return;
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        setAntiAliasOn();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        float borderColor[] = { 0.f, 1.f, 0.f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        data = (ByteData)malloc(size*4);


        fix sizeMB = size*4/(1024*1024.);
        Log::Critical() << "OpenGL::Texture is allocating " << sizeMB << "MB of GPU texture data to upload "
                        << size << " history to." << Log::Flush;
        glTexImage1D(GL_TEXTURE_2D, 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        auto error = glGetError();
        if(error != GL_NO_ERROR)
            Log::Error() << "OpenGL::Texture failed to allocate " << sizeMB << "MB of GPU texture data." << Log::Flush;
        else
            Log::Success() << "OpenGL::Texture allocated " << sizeMB << "MB of GPU texture data." << Log::Flush;
    }

    bool Texture1D::setColor(int i, Styles::Color color) {
        if(data == nullptr) return false;

        fix index = i*4;
        ByteData texel = &data[index];

        texel[R] = (Byte)(255*color.r);
        texel[G] = (Byte)(255*color.g);
        texel[B] = (Byte)(255*color.b);
        texel[A] = (Byte)(255*color.a);

        return true;
    }

    bool Texture1D::upload(UInt row0, Count nRows) {
        if(data == nullptr || texture == 0) return false;

        glBindTexture(GL_TEXTURE_2D, texture);

        if(row0==0 && nRows==0) {
            glTexImage1D(GL_TEXTURE_2D, 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            auto error = glGetError();
            if(error != GL_NO_ERROR)
                Log::Error() << "OpenGL::Texture failed to upload " << size << " data to GL_TEXTURE_2D." << Log::Flush;
        }
        else {
            glTexSubImage1D(GL_TEXTURE_2D, 0, row0, nRows, GL_RGBA, GL_UNSIGNED_BYTE, &data[row0 * 4]);
        }

        return true;
    }

    bool Texture1D::setData(ByteData newData) {
        if(texture == 0) return false;

        glTexImage1D(GL_TEXTURE_2D, 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, newData);

        return true;
    }

    GLsizei Texture1D::getSize() const {
        return size;
    }

    bool Texture1D::bind() const {
        if(texture == 0) return false;

        glBindTexture(GL_TEXTURE_1D, texture);

        return true;
    }

    void Texture1D::setAntiAlias(bool val) {
        if(val) setAntiAliasOn();
        else setAntiAliasOff();
    }

    void Texture1D::setAntiAliasOn() {
        glBindTexture(GL_TEXTURE_1D, texture);

        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        antiAlias = true;
    }

    void Texture1D::setAntiAliasOff() {
        glBindTexture(GL_TEXTURE_1D, texture);

        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        antiAlias = false;
    }

    bool Texture1D::getAntiAlias() const {
        return antiAlias;
    }

    void Texture1D::set_sPeriodicOn() {
        glBindTexture(GL_TEXTURE_1D, texture);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    }


} // OpenGL