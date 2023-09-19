//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_COLORSTIFF_H
#define STUDIOSLAB_TEXTURE2D_COLORSTIFF_H

#include "Texture2D.h"
#include "Core/Graphics/Styles/Colors.h"

namespace OpenGL {

    class Texture2D_Color_STIFF : public Texture2D {

        ByteData data = nullptr;

        bool antiAlias = true;
    public:
        Texture2D_Color_STIFF(GLsizei width, GLsizei height);

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

#endif //STUDIOSLAB_TEXTURE2D_COLORSTIFF_H
