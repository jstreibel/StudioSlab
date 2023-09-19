//
// Created by joao on 16/09/23.
//

#ifndef STUDIOSLAB_TEXTURE1D_COLOR_H
#define STUDIOSLAB_TEXTURE1D_COLOR_H

#include "Texture.h"

#include "Utils/Types.h"

#include "Core/Graphics/OpenGL/OpenGL.h"
#include "Core/Graphics/Styles/Colors.h"

namespace OpenGL {

    class Texture1D_Color : public Texture {
        ByteData data = nullptr;

        GLsizei size;

        bool antiAlias = true;
    public:
        Texture1D_Color(GLsizei length);

        bool setColor(int i, Styles::Color color);

        bool getAntiAlias() const;
        void setAntiAlias(bool val);
        void setAntiAliasOn();
        void setAntiAliasOff();

        void set_sPeriodicOn();

        bool upload(UInt from=0, Count n=0);

        GLsizei getSize() const;
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE1D_COLOR_H
