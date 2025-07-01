//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_COLOR_H
#define STUDIOSLAB_TEXTURE2D_COLOR_H

#include "Texture2D.h"

#include "Utils/Types.h"

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/Styles/Colors.h"

namespace Slab::Graphics::OpenGL {

    class Texture2D_Color : public Texture2D {
        ByteData_raw data = nullptr;

    public:

        Texture2D_Color(GLsizei width, GLsizei height);

        bool setColor(int i, int j, FColor color);

        bool upload(UInt row=0, CountType nRows=0);
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE2D_COLOR_H
