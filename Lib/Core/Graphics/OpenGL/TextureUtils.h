//
// Created by joao on 19/09/23.
//

#ifndef STUDIOSLAB_TEXTUREUTILS_H
#define STUDIOSLAB_TEXTUREUTILS_H

#include "OpenGL.h"
#include "Utils/Types.h"

namespace Graphics::OpenGL {
    enum WrapMode {
         ClampToEdge        = GL_CLAMP_TO_EDGE,
         ClampToBorder      = GL_CLAMP_TO_BORDER,
         MirroredRepeat     = GL_MIRRORED_REPEAT,
         Repeat             = GL_REPEAT,
         MirrorClampToEdge  = GL_MIRROR_CLAMP_TO_EDGE
    };

    enum Target {
        Texture_1D                  = GL_TEXTURE_1D,
        Texture_2D                  = GL_TEXTURE_2D,
        Texture_3D                  = GL_TEXTURE_3D,
        Texture_1D_Array            = GL_TEXTURE_1D_ARRAY,
        Texture_2D_Array            = GL_TEXTURE_2D_ARRAY,
        Texture_Rectangle           = GL_TEXTURE_RECTANGLE,
        Texture_CubeMap             = GL_TEXTURE_CUBE_MAP,
        Texture_CubeMap_Array       = GL_TEXTURE_CUBE_MAP_ARRAY,
        Texture_Buffer              = GL_TEXTURE_BUFFER,
        Texture_2D_Multisample      = GL_TEXTURE_2D_MULTISAMPLE,
        Texture_2D_MultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    };

    enum InternalFormat {
        Depth = GL_DEPTH_COMPONENT,
        Stencil = GL_DEPTH_STENCIL,
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA,

        Red_8bit = GL_R8	,
        // GL_R8_SNORM,
        // GL_R16,
        // GL_R16_SNORM,
        // GL_RG8	GL_RG,
        // GL_RG8_SNORM,
        // GL_RG16	GL_RG,
        // GL_RG16_SNORM,
        // GL_R3_G3_B2,
        // GL_RGB4,
        // GL_RGB5,
        // GL_RGB8,
        // GL_RGB8_SNORM,
        // GL_RGB10,
        // GL_RGB12,
        // GL_RGB16_SNORM,
        // GL_RGBA2,
        // GL_RGBA4,
        // GL_RGB5_A1,
        // GL_RGBA8,
        // GL_RGBA8_SNORM,
        // GL_RGB10_A2,
        // GL_RGB10_A2UI,
        // GL_RGBA12,
        // GL_RGBA16,
        // GL_SRGB8,
        // GL_SRGB8_ALPHA8,
        // GL_R16F,
        // GL_RG16F,
        // GL_RGB16F,
        // GL_RGBA16F,
        Red_32bit_Float = GL_R32F,
        // GL_RG32F,
        // GL_RGB32F,
        // GL_RGBA32F,
        // GL_R11F_G11F_B10F,
        // GL_RGB9_E5,
        // GL_R8I,
        // GL_R8UI,
        // GL_R16I,
        // GL_R16UI,
        // GL_R32I,
        // GL_R32UI,
        // GL_RG8I,
        // GL_RG8UI,
        // GL_RG16I,
        // GL_RG16UI,
        // GL_RG32I,
        // GL_RG32UI,
        // GL_RGB8I,
        // GL_RGB8UI,
        // GL_RGB16I,
        // GL_RGB16UI,
        // GL_RGB32I,
        // GL_RGB32UI,
        // GL_RGBA8I,
        // GL_RGBA8UI,
        // GL_RGBA16I,
        // GL_RGBA16UI,
        // GL_RGBA32I,
        // GL_RGBA32UI

        // There's also table 3 from https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    };

    Str TargetToString(Target target);

    Str InternalFormatToString(InternalFormat format);

    Str TextureUnitToString(GLenum unit);

} // OpenGL

#endif //STUDIOSLAB_TEXTUREUTILS_H
