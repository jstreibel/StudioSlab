//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_TEXTURE_H
#define STUDIOSLAB_TEXTURE_H

#include "Utils/Types.h"

#include "Core/Graphics/OpenGL/OpenGL.h"
#include "Core/Graphics/Styles/Colors.h"

namespace OpenGL {

    class Texture {
        GLuint texture = 0;
        ByteData data = nullptr;

        GLsizei w, h;
    public:
        Texture(GLsizei width, GLsizei height);

        bool setColor(int i, int j, Styles::Color color);
        bool setData(ByteData data);

        bool bind() const;

        bool upload(PosInt row=0, Count nRows=0);

        GLsizei getWidth() const;
        GLsizei getHeight() const;
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE_H