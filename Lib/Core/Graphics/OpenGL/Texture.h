//
// Created by joao on 19/09/23.
//

#ifndef STUDIOSLAB_TEXTURE_H
#define STUDIOSLAB_TEXTURE_H

#include "Utils/Types.h"
#include "Core/Graphics/OpenGL/OpenGL.h"

namespace OpenGL {

    class Texture {
    protected:
        GLuint texture = 0;
    public:
        Texture();
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE_H
