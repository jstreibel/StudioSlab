//
// Created by joao on 19/09/23.
//

#ifndef STUDIOSLAB_TEXTURE_H
#define STUDIOSLAB_TEXTURE_H

#include "Utils/Types.h"

#include "OpenGL.h"
#include "TextureUtils.h"


namespace OpenGL {

    class Texture {
    protected:
        GLuint texture = 0;
        Target target;
        InternalFormat internalFormat;

    public:
        Texture(Target, InternalFormat);

        void bind() const;
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE_H
