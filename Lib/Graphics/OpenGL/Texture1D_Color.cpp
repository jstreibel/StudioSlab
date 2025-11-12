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

namespace Slab::Graphics::OpenGL {

    using Log = Core::Log;

    Texture1D_Color::Texture1D_Color(GLsizei size, GLenum textureUnit)
    : FTexture(Texture_1D, RGBA, textureUnit)
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
        Bind();

        setAntiAliasOn();

        data = (ByteData_raw)malloc(size*4);

        fix sizeMB = size*4/(1024*1024.);
        glTexImage1D(Texture_1D, 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        if(!CheckGLErrors("after trying to reserve texture in gpu"))
            Log::Note() << "OpenGL::Texture allocated " << sizeMB << "MB of GPU texture data." << Log::Flush;
    }

    bool Texture1D_Color::setColor(int i, FColor color) {
        if(data == nullptr) return false;

        fix index = i*4;
        ByteData_raw texel = &data[index];

        texel[R] = (Byte)(255*color.r);
        texel[G] = (Byte)(255*color.g);
        texel[B] = (Byte)(255*color.b);
        texel[A] = (Byte)(255*color.a);

        return true;
    }

    bool Texture1D_Color::upload(UInt start, CountType n) {
        if(data == nullptr) return false;

        Bind();

        if(start==0 && n==0) {
            glTexImage1D(GetTarget(), 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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
        Bind();

        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        antiAlias = true;
    }

    void Texture1D_Color::setAntiAliasOff() {
        Bind();

        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        antiAlias = false;
    }

    bool Texture1D_Color::getAntiAlias() const { return antiAlias; }

    void Texture1D_Color::set_sPeriodicOn() const { setWrap(Repeat); }

    void Texture1D_Color::setWrap(OpenGL::WrapMode mode) const {
        Bind();

        glTexParameteri(GetTarget(), GL_TEXTURE_WRAP_S, mode);
    }


} // OpenGL