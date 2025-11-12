//
// Created by joao on 17/09/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_H
#define STUDIOSLAB_TEXTURE2D_H

#include "Texture.h"

namespace Slab::Graphics::OpenGL {

    class FTexture2D : public FTexture {
        GLsizei w, h;

        bool b_AntiAlias = true;

    public:
        FTexture2D(GLsizei w, GLsizei h, InternalFormat format, GLenum textureUnit = GL_TEXTURE0);

        static GLint GetMaxTextureSize();

        GLsizei GetWidth() const;
        GLsizei GetHeight() const;

        bool GetAntiAlias() const;
        void SetAntiAlias(bool val);
        void SetAntiAliasOn();
        void SetAntiAliasOff();

        bool UploadData(UInt row0, CountType nRows, PixelDataFormat format, PixelDataType type, const void *dataBegin);

        void Set_sPeriodicOn();
        void SetSWrap(WrapMode wrapMode);

        void Set_tPeriodicOn();
        void SetTWrap(WrapMode wrapMode);
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE2D_H
