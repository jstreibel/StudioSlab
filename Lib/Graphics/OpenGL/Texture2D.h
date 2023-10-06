//
// Created by joao on 17/09/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_H
#define STUDIOSLAB_TEXTURE2D_H

#include "Texture.h"

namespace Graphics::OpenGL {

    class Texture2D : public Texture {
        GLsizei w, h;

        bool antiAlias = true;

    public:
        Texture2D(GLsizei w, GLsizei h, InternalFormat format, GLenum textureUnit = GL_TEXTURE0);

        GLsizei getWidth() const;
        GLsizei getHeight() const;

        bool getAntiAlias() const;
        void setAntiAlias(bool val);
        void setAntiAliasOn();
        void setAntiAliasOff();

        bool uploadData(UInt row0, Count nRows, PixelDataFormat format, PixelDataType type, const void *dataBegin);

        void set_sPeriodicOn();
        void setSWrap(WrapMode wrapMode);

        void set_tPeriodicOn();
        void setTWrap(WrapMode wrapMode);
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE2D_H
