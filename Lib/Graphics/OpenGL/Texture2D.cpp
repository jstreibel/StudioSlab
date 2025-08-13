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
        auto maxTextureSize = GetMaxTextureSize();

        if(w>maxTextureSize || h>maxTextureSize) {
            Log::Error() << "Requested texture size " << w << "x" << h
                         << " too big: max texture size allowed is " << maxTextureSize << "x" << maxTextureSize
                         << Log::Flush;

            throw Exception("Texture too big");
        }

        Bind();

        // Log::Critical() << "OpenGL::Texture is reserving GPU texture space to upload " << w << "x" << h << " pixels to." << Log::Flush;

        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, UspecifiedFormat, UnspecifiedType, nullptr);

        setAntiAliasOn();

        if(CheckGLErrors("reserve " + ToStr(w) + "x" + ToStr(h) + " GPU texture pixels"))
            Log::Error() << "OpenGL::Texture failed reserving " << w << "x" << h << " GPU texture pixels." << Log::Flush;
    }

    GLsizei Texture2D::getWidth() const { return w; }

    GLsizei Texture2D::getHeight() const { return h; }

    void Texture2D::setAntiAlias(bool val) {
        if(val) setAntiAliasOn();
        else setAntiAliasOff();
    }

    void Texture2D::setAntiAliasOn() {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        antiAlias = true;
    }

    void Texture2D::setAntiAliasOff() {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        antiAlias = false;
    }

    bool Texture2D::getAntiAlias() const {
        return antiAlias;
    }

    void Texture2D::set_sPeriodicOn() {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode::Repeat);
    }

    void Texture2D::setSWrap(WrapMode wrapMode) {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    }

    void Texture2D::set_tPeriodicOn() {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode::Repeat);
    }

    void Texture2D::setTWrap(WrapMode wrapMode) {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    }

    bool Texture2D::uploadData(UInt row0, CountType nRows, PixelDataFormat dataFormat,
                               PixelDataType dataType, const void *dataBegin) {
        assert(row0<getHeight());
        assert(getTarget() == GL_TEXTURE_2D);
        assert(!((nRows==0) && (row0!=0)));

        if(nRows==0) nRows = h;
        fix column0 = 0;
        fix nCols = w;
        fix level = 0;

        CheckGLErrors(__PRETTY_FUNCTION__);

        fix internalFormat = getInternalFormat();

        Bind();

        if(row0==0 && nRows==w)
            glTexImage2D(getTarget(), level, internalFormat,
                         nCols, (GLsizei)nRows, 0,
                         dataFormat, dataType, dataBegin);
        else
            glTexSubImage2D(getTarget(), level,
                            (GLint)   column0, (GLint)   row0,
                            (GLsizei) nCols,   (GLsizei) nRows,
                            dataFormat, dataType, dataBegin);

        if(CheckGLErrors(__PRETTY_FUNCTION__, false)) {
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

    GLint Texture2D::GetMaxTextureSize() {
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        return maxTextureSize;
    }

} // OpenGL