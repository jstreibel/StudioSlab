//
// Created by joao on 16/09/23.
//

#include "Texture2D_Real.h"
#include "Core/Tools/Log.h"

namespace OpenGL {

    Texture2D_Real::Texture2D_Real(GLsizei width, GLsizei height) : w(width), h(height)
    {
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        if(width>maxTextureSize || height>maxTextureSize) {
            Log::Error() << "Requested texture size " << width << "x" << height
                         << " too big: max texture size allowed is " << maxTextureSize << "x" << maxTextureSize
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

        data = (RealData)malloc(w*h*sizeof(Real));

        fix sizeMB = w*h*sizeof(Real)/(1024*1024.);
        Log::Critical() << "OpenGL::Texture is allocating " << sizeMB << "MB of GPU texture data to upload " << w << "x" << h << " history to." << Log::Flush;
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, pixelDataFormat, pixelDataType, nullptr);
        auto error = glGetError();
        if(error != GL_NO_ERROR)
            Log::Error() << "OpenGL::Texture failed to allocate " << sizeMB << "MB of GPU texture data." << Log::Flush;
        else
            Log::Success() << "OpenGL::Texture allocated " << sizeMB << "MB of GPU texture data." << Log::Flush;
    }

    bool Texture2D_Real::setValue(int i, int j, Real value) {
        if(data == nullptr) return false;

        fix index = i + j*w;
        RealData texel = &data[index];

        texel[0] = (Real)value;

        return true;
    }

    bool Texture2D_Real::upload(UInt row0, Count nRows) {
        if(data == nullptr || texture == 0) return false;

        glBindTexture(GL_TEXTURE_2D, texture);

        if(row0==0 && nRows==0) {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, pixelDataFormat, pixelDataType, data);
            auto error = glGetError();
            if(error != GL_NO_ERROR)
                Log::Error() << "OpenGL::Texture failed to upload " << w << "x" << h << " data to GL_TEXTURE_2D." << Log::Flush;
        }
        else {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row0, w, nRows, pixelDataFormat, pixelDataType, &data[row0 * w * 4]);
        }

        return true;
    }

    bool Texture2D_Real::setData(RealData newData) {
        if(texture == 0) return false;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, pixelDataFormat, pixelDataType, newData);

        return true;
    }

    GLsizei Texture2D_Real::getWidth() const {
        return w;
    }

    GLsizei Texture2D_Real::getHeight() const {
        return h;
    }

    bool Texture2D_Real::bind() const {
        if(texture == 0) return false;

        glBindTexture(GL_TEXTURE_2D, texture);

        return true;
    }

    void Texture2D_Real::setAntiAlias(bool val) {
        if(val) setAntiAliasOn();
        else setAntiAliasOff();
    }

    void Texture2D_Real::setAntiAliasOn() {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        antiAlias = true;
    }

    void Texture2D_Real::setAntiAliasOff() {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        antiAlias = false;
    }

    bool Texture2D_Real::getAntiAlias() const {
        return antiAlias;
    }

    void Texture2D_Real::set_sPeriodicOn() {
        glBindTexture(GL_TEXTURE_2D, texture);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    }

} // OpenGL