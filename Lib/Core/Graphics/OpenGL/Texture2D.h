//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_H
#define STUDIOSLAB_TEXTURE2D_H

#include "Utils/Types.h"

#include "Core/Graphics/OpenGL/OpenGL.h"
#include "Core/Graphics/Styles/Colors.h"

namespace OpenGL {

    class Texture2D {
        GLuint texture = 0;
        ByteData data = nullptr;

        GLsizei w, h;

        bool antiAlias = true;
    public:
        Texture2D(GLsizei width, GLsizei height);

        bool setColor(int i, int j, Styles::Color color);
        bool setData(ByteData data);

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

#endif //STUDIOSLAB_TEXTURE2D_H
