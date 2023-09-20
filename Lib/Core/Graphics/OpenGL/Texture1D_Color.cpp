//
// Created by joao on 16/09/23.
//

#include "Texture1D_Color.h"
#include "Core/Tools/Log.h"
#include "Utils.h"

#define R 0
#define G 1
#define B 2
#define A 3

namespace OpenGL {

    Texture1D_Color::Texture1D_Color(GLsizei size, GLenum textureUnit)
    : Texture(Texture_1D, RGBA, textureUnit)
    , size(size)
    {
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        if(size>maxTextureSize) {
            Log::Error() << "Requested texture size " << size
                         << " too big: max texture size allowed is " << maxTextureSize
                         << Log::Flush;

            return;
        }
        bind();

        setAntiAliasOn();

        glTexParameteri(Texture_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 0.f, 1.f, 0.f, 1.0f };
        glTexParameterfv(Texture_1D, GL_TEXTURE_BORDER_COLOR, borderColor);

        data = (ByteData)malloc(size*4);

        fix sizeMB = size*4/(1024*1024.);
        Log::Critical() << "OpenGL::Texture is allocating " << sizeMB << "MB of GPU texture data to upload "
                        << size << " history to." << Log::Flush;
        glTexImage1D(Texture_1D, 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        if(!OpenGLUtils::checkGLErrors("after trying to reserve texture in gpu"))
            Log::Success() << "OpenGL::Texture allocated " << sizeMB << "MB of GPU texture data." << Log::Flush;
    }

    bool Texture1D_Color::setColor(int i, Styles::Color color) {
        if(data == nullptr) return false;

        fix index = i*4;
        ByteData texel = &data[index];

        texel[R] = (Byte)(255*color.r);
        texel[G] = (Byte)(255*color.g);
        texel[B] = (Byte)(255*color.b);
        texel[A] = (Byte)(255*color.a);

        return true;
    }

    bool Texture1D_Color::upload(UInt start, Count n) {
        if(data == nullptr) return false;

        bind();

        if(start==0 && n==0) {
            glTexImage1D(getTarget(), 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            auto error = glGetError();
            if(error != GL_NO_ERROR)
                Log::Error() << "OpenGL::Texture failed to upload " << size << " data to GL_TEXTURE_2D." << Log::Flush;
        }
        else {
            glTexSubImage1D(GL_TEXTURE_1D, 0, (GLint)start, (GLint)n, GL_RGBA, GL_UNSIGNED_BYTE, &data[start * 4]);
        }

        return true;
    }

    GLsizei Texture1D_Color::getSize() const {
        return size;
    }

    void Texture1D_Color::setAntiAlias(bool val) {
        if(val) setAntiAliasOn();
        else setAntiAliasOff();
    }

    void Texture1D_Color::setAntiAliasOn() {
        bind();

        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        antiAlias = true;
    }

    void Texture1D_Color::setAntiAliasOff() {
        bind();

        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        antiAlias = false;
    }

    bool Texture1D_Color::getAntiAlias() const {
        return antiAlias;
    }

    void Texture1D_Color::set_sPeriodicOn() {
        bind();

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    }


} // OpenGL