//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_TEXTURE_H
#define STUDIOSLAB_TEXTURE_H

#include "GL/gl.h"
#include "Utils/Types.h"
#include "Core/Graphics/Styles/Colors.h"

namespace OpenGL {

    class Texture {
        GLuint texture = 0;
        ByteData data;

        GLsizei w, h;
    public:
        Texture(GLsizei width, GLsizei height);

        void setColor(int i, int j, Styles::Color color);
        void setData(ByteData data);

        void upload(PosInt row=0, Count nRows=0);

        GLsizei getWidth() const;
        GLsizei getHeight() const;
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE_H
