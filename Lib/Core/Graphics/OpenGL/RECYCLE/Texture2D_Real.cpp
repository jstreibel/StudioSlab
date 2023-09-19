//
// Created by joao on 16/09/23.
//

#include "Texture2D_Real.h"
#include "Core/Tools/Log.h"

namespace OpenGL {

    Texture2D_Real::Texture2D_Real(GLsizei w, GLsizei h)
    : Texture2D(w, h, InternalFormat::Red_32bit_Float)
    , data((RealData)malloc(w*h*sizeof(Real)))
    {
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        if(w>maxTextureSize || h>maxTextureSize) {
            Log::Error() << "Requested texture size " << w << "x" << h
                         << " too big: max texture size allowed is " << maxTextureSize << "x" << maxTextureSize
                         << Log::Flush;

            return;
        }
    }

    bool Texture2D_Real::setValue(int i, int j, Real value) {
        if(data == nullptr) return false;

        fix index = i + j*getWidth();
        RealData texel = &data[index];

        texel[0] = (Real)value;

        return true;
    }

} // OpenGL