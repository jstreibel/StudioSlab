//
// Created by joao on 19/09/23.
//

#include "TextureUtils.h"

namespace OpenGL {

    Str TargetToString(Target target) {
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

    Str InternalFormatToString(InternalFormat format) {
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

    Str TextureUnitToString(GLenum unit) {
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

} // OpenGL