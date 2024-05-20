//
// Created by joao on 17/09/23.
//

#include "Texture.h"
#include "Utils/Utils.h"
#include "Core/Tools/Log.h"
#include "Utils.h"

#define CHECK_GL_ERRORS(count) checkGLErrors(Str(__PRETTY_FUNCTION__) \
    + " from " + Common::getClassName(this)                                        \
    + " (check " + ToStr((count)) + ")");

namespace Graphics::OpenGL {
    Texture::Texture(Target target, InternalFormat format, GLenum textureUnit)
    : target(target)
    , format(format)
    , textureUnit(textureUnit)
    {
        glGenTextures(1, &handle);

        Log::Debug() << "OpenGL::Texture " << handle << " generated. Texture unit: " << TextureUnitToString(textureUnit) << " (" << (textureUnit-GL_TEXTURE0) << ")" << Log::Flush;
    }

    void Texture::activate() const {
        glActiveTexture(getGLtextureUnit());
    }

    void Texture::deactivate() const {
        glActiveTexture(GL_TEXTURE0);
    }

    void Texture::bind() const {
        activate();
        CHECK_GL_ERRORS(0)
        glBindTexture(target, handle);
        CHECK_GL_ERRORS(1)
    }

    GLuint Texture::getHandle()         const { return handle; }

    GLint Texture::getTextureUnit()     const { return (int)textureUnit-GL_TEXTURE0; }

    GLuint Texture::getGLtextureUnit()  const { return textureUnit; }

    void Texture::setBorderColor(Color color) const {
        float borderColor[] = { color.r, color.g, color.b, color.a };
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    auto Texture::getData() const -> const PixelData & { return *pixelData; }

    void Texture::setData(const PixelData &data) {
        bind();

        if(data.nDim == 1){
            assert(target == GL_TEXTURE_1D);

            glTexImage1D(target, 0, format, data.width, 0, data.dataFormat, data.dataType, data.getData());

            return;
        }

        else if(data.nDim == 2){
            assert(target == GL_TEXTURE_2D);

            glTexImage2D(target, 0, format, data.width, data.height, 0, data.dataFormat, data.dataType, data.getData());
        }

        else if(data.nDim == 3){
            assert(target == GL_TEXTURE_3D);

            glTexImage3D(target, 0, format, data.width, data.height, data.depth, 0, data.dataFormat, data.dataType, data.getData());
        }

        throw Str("No such dimensions ") + ToStr(data.nDim) + " for OpenGL textures";
    }

    auto Texture::getTarget() const -> Target { return target; }

    auto Texture::getInternalFormat() const -> InternalFormat { return format; }

    void Texture::diagnose() const {

        GLint currentTexture = 0;
        GLenum bindingEnum;

        switch(target) {
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
        glBindTexture(target, handle);

        // Query dimensions
        GLint width, height;
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);

        // Query internal format
        GLint internalFormat;
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
        auto iFormat = (InternalFormat)internalFormat;

        fix pass = currentTexture==handle && iFormat==format;
        Log::Debug() << "OpenGL::Texture diagnose [" << (pass?Log::FGGreen+"PASS":Log::FGRed+"FAIL") << Log::ResetFormatting + "]: actual [expected]; "
                     << "dimensions " << width << "x" << height << " [...]; "
                     << "bound texture id " << currentTexture << " [" << handle << "]; "
                     << "internal format " << InternalFormatToString(iFormat) << " [" << InternalFormatToString(format) << "]; "
                     << Log::Flush;

        // Restore previously bound texture
        glBindTexture(target, currentTexture);
    }


} // OpenGL