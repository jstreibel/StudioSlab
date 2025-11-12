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

    FTexture2D::FTexture2D(GLsizei w, GLsizei h, InternalFormat format, GLenum textureUnit)
    : FTexture(Texture_2D, format, textureUnit)
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

        SetAntiAliasOn();

        if(CheckGLErrors("reserve " + ToStr(w) + "x" + ToStr(h) + " GPU texture pixels"))
            Log::Error() << "OpenGL::Texture failed reserving " << w << "x" << h << " GPU texture pixels." << Log::Flush;
    }

    GLsizei FTexture2D::GetWidth() const { return w; }

    GLsizei FTexture2D::GetHeight() const { return h; }

    void FTexture2D::SetAntiAlias(bool val) {
        if(val) SetAntiAliasOn();
        else SetAntiAliasOff();
    }

    void FTexture2D::SetAntiAliasOn() {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        b_AntiAlias = true;
    }

    void FTexture2D::SetAntiAliasOff() {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        b_AntiAlias = false;
    }

    bool FTexture2D::GetAntiAlias() const {
        return b_AntiAlias;
    }

    void FTexture2D::Set_sPeriodicOn() {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode::Repeat);
    }

    void FTexture2D::SetSWrap(WrapMode wrapMode) {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    }

    void FTexture2D::Set_tPeriodicOn() {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode::Repeat);
    }

    void FTexture2D::SetTWrap(WrapMode wrapMode) {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    }

    bool FTexture2D::UploadData(UInt row0, CountType nRows, PixelDataFormat dataFormat,
                               PixelDataType dataType, const void *dataBegin) {
        assert(row0<GetHeight());
        assert(GetTarget() == GL_TEXTURE_2D);
        assert(!((nRows==0) && (row0!=0)));

        if(nRows==0) nRows = h;
        fix column0 = 0;
        fix nCols = w;
        fix level = 0;

        CheckGLErrors(__PRETTY_FUNCTION__);

        fix internalFormat = GetInternalFormat();

        Bind();

        if(row0==0 && nRows==w)
            glTexImage2D(GetTarget(), level, internalFormat,
                         nCols, (GLsizei)nRows, 0,
                         dataFormat, dataType, dataBegin);
        else
            glTexSubImage2D(GetTarget(), level,
                            (GLint)   column0, (GLint)   row0,
                            (GLsizei) nCols,   (GLsizei) nRows,
                            dataFormat, dataType, dataBegin);

        if(CheckGLErrors(__PRETTY_FUNCTION__, false)) {
            Log::Error() << "OpenGL::Texture failed to upload " << w << "x" << h
                         << " data to target " << TargetToString(GetTarget())
                         << " with internal format " << InternalFormatToString(GetInternalFormat())
                         << " @ unit " << GetTextureUnit() << " (" << TextureUnitToString(GetGLtextureUnit()) << ")."
                         << " The input pixel format was " << PixelData::PixelDataFormatToString(dataFormat)
                         << " and data type " << PixelData::PixelDataTypeToString(dataType)
                         << Log::Flush;

            FTexture::Diagnose();

            return false;
        }

        return true;
    }

    GLint FTexture2D::GetMaxTextureSize() {
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        return maxTextureSize;
    }

} // OpenGL