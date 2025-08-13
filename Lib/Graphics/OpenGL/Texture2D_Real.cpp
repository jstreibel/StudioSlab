//
// Created by joao on 16/09/23.
//

#include "Texture2D_Real.h"
#include "Core/Tools/Log.h"

namespace Slab::Graphics::OpenGL {

    Texture2D_Real::Texture2D_Real(GLsizei w, GLsizei h)
    : Texture2D(w, h, InternalFormat::Red_32bit_Float)
    , data((RealData)malloc(w*h*sizeof(Real32)))
    {

    }

    bool Texture2D_Real::setValue(int i, int j, Real32 value) {
        if(data == nullptr) return false;

        fix index = i + j*getWidth();
        data[index] = value;

        return true;
    }

    bool Texture2D_Real::upload(UInt row0, CountType nRows) {
        if(data == nullptr) return false;

        assert(getInternalFormat() == InternalFormat::Red_32bit_Float);

        fix dataFormat = PixelDataFormat::DataFormat_Red;
        fix dataType = PixelDataType::DataType_Float32;

        return Texture2D::uploadData(row0, nRows, dataFormat, dataType, &data[row0 * getWidth()]);
    }

} // OpenGL