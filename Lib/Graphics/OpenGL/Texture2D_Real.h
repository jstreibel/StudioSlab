//
// Created by joao on 16/09/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_REAL_H
#define STUDIOSLAB_TEXTURE2D_REAL_H

#include "Utils/Types.h"

#include "Texture2D.h"

namespace Slab::Graphics::OpenGL {

    class Texture2D_Real : public Texture2D {
        RealData data = nullptr;

    public:
        Texture2D_Real(GLsizei width, GLsizei height);

        bool setValue(int i, int j, Real32 value);

        bool upload(UInt row=0, Count nRows=0);
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE2D_REAL_H
