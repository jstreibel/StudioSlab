//
// Created by joao on 16/09/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_REAL_H
#define STUDIOSLAB_TEXTURE2D_REAL_H

#include "Utils/Types.h"

#include "Core/Graphics/OpenGL/OpenGL.h"

namespace OpenGL {

    class Texture2D_Real {
        GLuint texture = 0;
        RealData data = nullptr;

        const GLint internalFormat = GL_R32F;
        const GLuint pixelDataFormat = GL_RED;
        const GLuint pixelDataType = GL_FLOAT;

        GLsizei w, h;

        bool antiAlias = true;
    public:
        Texture2D_Real(GLsizei width, GLsizei height);

        bool setValue(int i, int j, Real value);
        bool setData(RealData data);

        bool getAntiAlias() const;
        void setAntiAlias(bool val);
        void setAntiAliasOn();
        void setAntiAliasOff();

        void set_sPeriodicOn();

        bool bind() const;

        bool upload(UInt row=0, Count nRows=0);

        GLsizei getWidth() const;
        GLsizei getHeight() const;
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE2D_REAL_H
