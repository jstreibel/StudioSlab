//
// Created by joao on 17/09/23.
//

#include "Texture2D.h"
#include "Core/Tools/Log.h"
#include "Utils.h"
#include "TextureUtils.h"


namespace Slab::Graphics::OpenGL {
    fix UspecifiedFormat = OpenGL::PixelDataFormat::DataFormat_Red;
    fix UnspecifiedType  = OpenGL::PixelDataType::DataType_Float32;

    using Log = Core::Log;

    Texture2D::Texture2D(GLsizei w, GLsizei h, InternalFormat format, GLenum textureUnit)
    : Texture(Texture_2D, format, textureUnit)
    , w(w)
    , h(h)
    {
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        if(w>maxTextureSize || h>maxTextureSize) {
            Log::Error() << "Requested texture size " << w << "x" << h
                         << " too big: max texture size allowed is " << maxTextureSize << "x" << maxTextureSize
                         << Log::Flush;

            throw "Texture too big";
        }

        bind();

        Log::Critical() << "OpenGL::Texture is reserving GPU texture space to upload "
                        << w << "x" << h << " pixels to." << Log::Flush;

        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, UspecifiedFormat, UnspecifiedType, nullptr);

        setAntiAliasOn();

        if(!checkGLErrors("reserve " + ToStr(w) + "x" + ToStr(h) + " GPU texture pixels"))
            Log::Success() << "OpenGL::Texture reserved " << w << "x" << h << " GPU texture pixels." << Log::Flush;
    }

    GLsizei Texture2D::getWidth() const { return w; }

    GLsizei Texture2D::getHeight() const { return h; }

    void Texture2D::setAntiAlias(bool val) {
        if(val) setAntiAliasOn();
        else setAntiAliasOff();
    }

    void Texture2D::setAntiAliasOn() {
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        antiAlias = true;
    }

    void Texture2D::setAntiAliasOff() {
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        antiAlias = false;
    }

    bool Texture2D::getAntiAlias() const {
        return antiAlias;
    }

    void Texture2D::set_sPeriodicOn() {
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode::Repeat);
    }

    void Texture2D::setSWrap(WrapMode wrapMode) {
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    }

    void Texture2D::set_tPeriodicOn() {
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode::Repeat);
    }

    void Texture2D::setTWrap(WrapMode wrapMode) {
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    }

    bool Texture2D::uploadData(UInt row0, Count nRows, PixelDataFormat dataFormat,
                               PixelDataType dataType, const void *dataBegin) {
        assert(row0<getHeight());
        assert(getTarget() == GL_TEXTURE_2D);
        assert(!((nRows==0) && (row0!=0)));

        if(nRows==0) nRows = h;
        fix column0 = 0;
        fix nCols = w;
        fix level = 0;

        checkGLErrors(__PRETTY_FUNCTION__);

        fix internalFormat = getInternalFormat();

        bind();

        if(row0==0 && nRows==w)
            glTexImage2D(getTarget(), level, internalFormat,
                         nCols, (GLsizei)nRows, 0,
                         dataFormat, dataType, dataBegin);
        else
            glTexSubImage2D(getTarget(), level,
                            (GLint)   column0, (GLint)   row0,
                            (GLsizei) nCols,   (GLsizei) nRows,
                            dataFormat, dataType, dataBegin);

        if(checkGLErrors(__PRETTY_FUNCTION__, false)) {
            Log::Error() << "OpenGL::Texture failed to upload " << w << "x" << h
                         << " data to target " << TargetToString(getTarget())
                         << " with internal format " << InternalFormatToString(getInternalFormat())
                         << " @ unit " << getTextureUnit() << " (" << TextureUnitToString(getGLtextureUnit()) << ")."
                         << " The input pixel format was " << PixelData::PixelDataFormatToString(dataFormat)
                         << " and data type " << PixelData::PixelDataTypeToString(dataType)
                         << Log::Flush;

            Texture::diagnose();

            return false;
        }

        return true;
    }

} // OpenGL