//
// Created by joao on 17/09/23.
//

#include "Texture.h"
#include "Utils/Utils.h"
#include "Core/Tools/Log.h"
#include "Utils.h"

#define CHECK_GL_ERRORS(count) OpenGLUtils::checkGLErrors(Str(__PRETTY_FUNCTION__) + " from " + Common::getClassName(this) + " (" + ToStr((count)) + ")");

namespace OpenGL {
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

    void Texture::bind() const {
        activate();
        CHECK_GL_ERRORS(0)
        glBindTexture(target, handle);
        CHECK_GL_ERRORS(0)
    }

    GLuint Texture::getHandle()         const { return handle; }

    GLint Texture::getTextureUnit()     const { return (int)textureUnit-GL_TEXTURE0; }

    GLuint Texture::getGLtextureUnit()  const { return textureUnit; }

    void Texture::setBorderColor(Styles::Color color) const {
        float borderColor[] = { color.r, color.g, color.b, color.a };
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    auto Texture::getData() const -> const PixelData & {
        return *pixelData;
    }

    /*
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
     */

    auto Texture::getTarget() const -> Target { return target; }

    auto Texture::getInternalFormat() const -> InternalFormat { return format; }

    Str Texture::TargetToString(Target target) {
        switch (target) {
            case Texture_1D                  : return "GL_TEXTURE_1D";
            case Texture_2D                  : return "GL_TEXTURE_2D";
            case Texture_3D                  : return "GL_TEXTURE_3D";
            case Texture_1D_Array            : return "GL_TEXTURE_1D_ARRAY";
            case Texture_2D_Array            : return "GL_TEXTURE_2D_ARRAY";
            case Texture_Rectangle           : return "GL_TEXTURE_RECTANGLE";
            case Texture_CubeMap             : return "GL_TEXTURE_CUBE_MAP";
            case Texture_CubeMap_Array       : return "GL_TEXTURE_CUBE_MAP_ARRAY";
            case Texture_Buffer              : return "GL_TEXTURE_BUFFER";
            case Texture_2D_Multisample      : return "GL_TEXTURE_2D_MULTISAMPLE";
            case Texture_2D_MultisampleArray : return "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";
        }

        return "unknown/unimplemented format";
    }

    Str Texture::InternalFormatToString(InternalFormat format) {
        switch (format) {
            case Depth           : return "GL_DEPTH_COMPONENT";
            case Stencil         : return "GL_DEPTH_STENCIL";
            case Red             : return "GL_RED";
            case RG              : return "GL_RG";
            case RGB             : return "GL_RGB";
            case RGBA            : return "GL_RGBA";
            case Red_8bit        : return "GL_R8";
            case Red_32bit_Float : return "GL_R32F";
        }

        return "unknown/unimplemented format";
    }

    Str Texture::TextureUnitToString(GLenum unit) {
        switch (unit) {
            case GL_TEXTURE0:  return "GL_TEXTURE0";
            case GL_TEXTURE1:  return "GL_TEXTURE1";
            case GL_TEXTURE2:  return "GL_TEXTURE2";
            case GL_TEXTURE3:  return "GL_TEXTURE3";
            case GL_TEXTURE4:  return "GL_TEXTURE4";
            case GL_TEXTURE5:  return "GL_TEXTURE5";
            case GL_TEXTURE6:  return "GL_TEXTURE6";
            case GL_TEXTURE7:  return "GL_TEXTURE7";
            case GL_TEXTURE8:  return "GL_TEXTURE8";
            case GL_TEXTURE9:  return "GL_TEXTURE9";
            case GL_TEXTURE10: return "GL_TEXTURE10";
            case GL_TEXTURE11: return "GL_TEXTURE11";
            case GL_TEXTURE12: return "GL_TEXTURE12";
            case GL_TEXTURE13: return "GL_TEXTURE13";
            case GL_TEXTURE14: return "GL_TEXTURE14";
            case GL_TEXTURE15: return "GL_TEXTURE15";
            case GL_TEXTURE16: return "GL_TEXTURE16";
            case GL_TEXTURE17: return "GL_TEXTURE17";
            case GL_TEXTURE18: return "GL_TEXTURE18";
            case GL_TEXTURE19: return "GL_TEXTURE19";
            case GL_TEXTURE20: return "GL_TEXTURE20";
            case GL_TEXTURE21: return "GL_TEXTURE21";
            case GL_TEXTURE22: return "GL_TEXTURE22";
            case GL_TEXTURE23: return "GL_TEXTURE23";
            case GL_TEXTURE24: return "GL_TEXTURE24";
            case GL_TEXTURE25: return "GL_TEXTURE25";
            case GL_TEXTURE26: return "GL_TEXTURE26";
            case GL_TEXTURE27: return "GL_TEXTURE27";
            case GL_TEXTURE28: return "GL_TEXTURE28";
            case GL_TEXTURE29: return "GL_TEXTURE29";
            case GL_TEXTURE30: return "GL_TEXTURE30";
            case GL_TEXTURE31: return "GL_TEXTURE31";

            default: return "unknown/unimplemented unit";
        }
    }

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