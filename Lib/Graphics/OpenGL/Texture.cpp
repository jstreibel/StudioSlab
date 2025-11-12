//
// Created by joao on 17/09/23.
//

#include "Texture.h"
#include "Utils/Utils.h"
#include "Core/Tools/Log.h"
#include "Utils.h"

#define CHECK_GL_ERRORS(count) CheckGLErrors(Str(__PRETTY_FUNCTION__) \
    + " from " + Common::getClassName(this)                                        \
    + " (check " + ToStr((count)) + ")");

namespace Slab::Graphics::OpenGL {

    FTexture::FTexture(Target target, InternalFormat format, GLenum textureUnit)
    : m_Target(target)
    , m_Internalformat(format)
    , m_TextureUnit(textureUnit)
    {
        glGenTextures(1, &m_Handle);

        Core::Log::Debug() << "OpenGL::Texture " << m_Handle << " generated. Texture unit: " << TextureUnitToString(textureUnit) << " (" << (textureUnit-GL_TEXTURE0) << ")" << Core::Log::Flush;
    }

    FTexture::~FTexture() {
        glDeleteTextures(1, &m_Handle);
    }

    void FTexture::EnableTextures() {
        glEnable(GL_TEXTURE_2D);
    }

    void FTexture::DisableTextures() {
        glDisable(GL_TEXTURE_2D);
    }

    void FTexture::Activate() const {
        glActiveTexture(GetGLtextureUnit());
    }

    void FTexture::Deactivate() {
        glActiveTexture(GL_TEXTURE0);
    }

    void FTexture::Bind() const {
        Activate();
        CHECK_GL_ERRORS(0)
        glBindTexture(m_Target, m_Handle);
        CHECK_GL_ERRORS(1)
    }

    GLuint FTexture::GetHandle()         const { return m_Handle; }

    int FTexture::GetTextureUnit()     const { return (int)m_TextureUnit-GL_TEXTURE0; }

    GLuint FTexture::GetGLtextureUnit()  const { return m_TextureUnit; }

    void FTexture::SetBorderColor(FColor color) const {
        float borderColor[] = { color.r, color.g, color.b, color.a };
        glTexParameterfv(m_Target, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    auto FTexture::GetData() const -> const PixelData & { return *p_PixelData; }

    void FTexture::SetData(const PixelData &data) {
        Bind();

        if(data.nDim == 1){
            assert(m_Target == GL_TEXTURE_1D);

            glTexImage1D(m_Target, 0, m_Internalformat, data.width, 0, data.dataFormat, data.dataType, data.getData());

            return;
        }

        else if(data.nDim == 2){
            assert(m_Target == GL_TEXTURE_2D);

            glTexImage2D(m_Target, 0, m_Internalformat, data.width, data.height, 0, data.dataFormat, data.dataType, data.getData());
        }

        else if(data.nDim == 3){
            assert(m_Target == GL_TEXTURE_3D);

            glTexImage3D(m_Target, 0, m_Internalformat, data.width, data.height, data.depth, 0, data.dataFormat, data.dataType, data.getData());
        }

        throw Str("No such dimensions ") + ToStr(data.nDim) + " for OpenGL textures";
    }

    auto FTexture::GetTarget() const -> Target { return m_Target; }

    auto FTexture::GetInternalFormat() const -> InternalFormat { return m_Internalformat; }

    void FTexture::Diagnose() const {

        using Log = Core::Log;

        GLint currentTexture = 0;
        GLenum bindingEnum;

        switch(m_Target) {
            case Texture_1D: bindingEnum = GL_TEXTURE_BINDING_1D; break;
            case Texture_2D: bindingEnum = GL_TEXTURE_BINDING_2D; break;
            case Texture_3D: bindingEnum = GL_TEXTURE_BINDING_3D; break;
            // Add more cases as needed
            default:
                Log::Error() << "Diagnose error: unknown or unsupported texture target" << Log::Flush;
                return;
        }

        glGetIntegerv(bindingEnum, &currentTexture);

        // Bind the texture for querying
        glBindTexture(m_Target, m_Handle);

        // Query dimensions
        GLint width, height;
        glGetTexLevelParameteriv(m_Target, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(m_Target, 0, GL_TEXTURE_HEIGHT, &height);

        // Query internal format
        GLint internalFormat;
        glGetTexLevelParameteriv(m_Target, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
        auto iFormat = (InternalFormat)internalFormat;

        fix pass = currentTexture==m_Handle && iFormat==m_Internalformat;
        Log::Debug() << "OpenGL::Texture diagnose [" << (pass?Log::FGGreen+"PASS":Log::FGRed+"FAIL") << Log::ResetFormatting + "]: actual [expected]; "
                     << "dimensions " << width << "x" << height << " [...]; "
                     << "bound texture id " << currentTexture << " [" << m_Handle << "]; "
                     << "internal format " << InternalFormatToString(iFormat) << " [" << InternalFormatToString(m_Internalformat) << "]; "
                     << Log::Flush;

        // Restore previously bound texture
        glBindTexture(m_Target, currentTexture);
    }

} // OpenGL