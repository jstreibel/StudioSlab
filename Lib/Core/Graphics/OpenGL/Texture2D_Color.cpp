//
// Created by joao on 18/08/23.
//

#include "Utils.h"
#include "Texture2D_Color.h"

#define R 0
#define G 1
#define B 2
#define A 3

#define CHECK_GL_ERRORS(count) OpenGLUtils::checkGLErrors(Str(__PRETTY_FUNCTION__) + " from " + Common::getClassName(this) + " (" + ToStr((count)) + ")");


namespace OpenGL {
    Texture2D_Color::Texture2D_Color(GLsizei w, GLsizei h)
    : Texture2D(w, h, InternalFormat::RGBA)
    , data((ByteData)malloc(w*h*4))
    {
        setAntiAliasOn();
    }

    bool Texture2D_Color::setColor(int i, int j, Styles::Color color) {
        if(data == nullptr) return false;

        fix index = i*4 + j*getWidth()*4;
        ByteData texel = &data[index];

        texel[R] = (Byte)(255*color.r);
        texel[G] = (Byte)(255*color.g);
        texel[B] = (Byte)(255*color.b);
        texel[A] = (Byte)(255*color.a);

        return true;
    }

    bool Texture2D_Color::upload(UInt row0, Count nRows) {
        if(data == nullptr) return false;

        assert(getInternalFormat() == GL_RGBA);

        fix dataFormat = PixelDataFormat::DataFormat_RGBA;
        fix dataType = PixelDataType::DataType_UByte;

        return Texture2D::uploadData(row0, nRows, dataFormat, dataType, &data[row0 * getWidth() * 4]);
    }

} // OpenGL