//
// Created by joao on 19/09/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_H
#define STUDIOSLAB_TEXTURE2D_H


#include "Texture.h"

namespace OpenGL {

    class Texture2D : public Texture {
    protected:
        GLsizei w, h;

        Texture2D(GLsizei w, GLsizei h, InternalFormat);
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE2D_H
