//
// Created by joao on 16/09/23.
//

#ifndef STUDIOSLAB_TEXTURE1D_COLOR_H
#define STUDIOSLAB_TEXTURE1D_COLOR_H

#include "Texture.h"

#include "Utils/Types.h"

#include "OpenGL.h"
#include "TextureUtils.h"
#include "../Styles/Colors.h"

namespace Graphics::OpenGL {

    class Texture1D_Color : public Texture {
        ByteData data = nullptr;

        GLsizei size;

        bool antiAlias = true;
    public:
        Texture1D_Color(GLsizei length, GLenum textureUnit = GL_TEXTURE0);

        bool setColor(int i, Styles::Color color);

        void setWrap(OpenGL::WrapMode) const;

        bool getAntiAlias() const;
        void setAntiAlias(bool val);
        void setAntiAliasOn();
        void setAntiAliasOff();

        void set_sPeriodicOn() const;

        bool upload(UInt from=0, Count n=0);

        GLsizei getSize() const;
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE1D_COLOR_H
