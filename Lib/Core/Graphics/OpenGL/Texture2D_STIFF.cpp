//
// Created by joao on 18/08/23.
//

#include "Texture2D_STIFF.h"
#include "Core/Tools/Log.h"
#include "Utils.h"

#define R 0
#define G 1
#define B 2
#define A 3

namespace OpenGL {
    Texture2D_Color_STIFF::Texture2D_Color_STIFF(GLsizei w, GLsizei h)
    : Texture2D(w, h) {

        setAntiAliasOn();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        float borderColor[] = { 0.f, 1.f, 0.f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        data = (ByteData)malloc(w*h*4);

        // fix sizeMB = w*h*4/(1024*1024.);
        // Log::Critical() << "OpenGL::Texture is allocating " << sizeMB << "MB of GPU texture data to upload " << w << "x" << h << " history to." << Log::Flush;
        // auto pixelDataFormat = GL_RGBA;
        // auto pixelDataType = GL_UNSIGNED_BYTE;
        // // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, pixelDataFormat, pixelDataType, nullptr);
        // auto error = glGetError();
        // if(error != GL_NO_ERROR)
        //     Log::Error() << "OpenGL::Texture failed to allocate " << sizeMB << "MB of GPU texture data." << Log::Flush;
        // else
        //     Log::Success() << "OpenGL::Texture allocated " << sizeMB << "MB of GPU texture data." << Log::Flush;
    }

    bool Texture2D_Color_STIFF::setColor(int i, int j, Styles::Color color) {
        if(data == nullptr) return false;

        fix index = i*4 + j*w*4;
        ByteData texel = &data[index];

        texel[R] = (Byte)(255*color.r);
        texel[G] = (Byte)(255*color.g);
        texel[B] = (Byte)(255*color.b);
        texel[A] = (Byte)(255*color.a);

        return true;
    }

    bool Texture2D_Color_STIFF::upload(UInt row0, Count nRows) {
        if(data == nullptr || texture == 0) return false;

        glBindTexture(GL_TEXTURE_2D, texture);

        if(row0==0 && nRows==0) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            auto error = glGetError();
            if(error != GL_NO_ERROR)
                Log::Error() << "OpenGL::Texture failed to upload " << w << "x" << h << " data to GL_TEXTURE_2D." << Log::Flush;
        }
        else {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row0, w, nRows, GL_RGBA, GL_UNSIGNED_BYTE, &data[row0 * w * 4]);
        }

        return true;
    }

    bool Texture2D_Color_STIFF::setData(ByteData newData) {
        if(texture == 0) return false;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, newData);

        return true;
    }

    GLsizei Texture2D_Color_STIFF::getWidth() const {
        return w;
    }

    GLsizei Texture2D_Color_STIFF::getHeight() const {
        return h;
    }

    bool Texture2D_Color_STIFF::bind() const {
        if(texture == 0) return false;

        glBindTexture(GL_TEXTURE_2D, texture);

        return true;
    }

    void Texture2D_Color_STIFF::setAntiAlias(bool val) {
        if(val) setAntiAliasOn();
        else setAntiAliasOff();
    }

    void Texture2D_Color_STIFF::setAntiAliasOn() {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        antiAlias = true;
    }

    void Texture2D_Color_STIFF::setAntiAliasOff() {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        antiAlias = false;
    }

    bool Texture2D_Color_STIFF::getAntiAlias() const {
        return antiAlias;
    }

    void Texture2D_Color_STIFF::set_sPeriodicOn() {
        glBindTexture(GL_TEXTURE_2D, texture);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    }


} // OpenGL