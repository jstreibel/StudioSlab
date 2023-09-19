//
// Created by joao on 17/09/23.
//

#ifndef STUDIOSLAB_PIXELDATA_H
#define STUDIOSLAB_PIXELDATA_H

#include "OpenGL.h"
#include "Utils/Types.h"

namespace OpenGL {

    enum PixelDataFormat {
        DataFormat_Red = GL_RED,
        DataFormat_RG = GL_RG,
        DataFormat_RGB = GL_RGB,
        DataFormat_BGR = GL_BGR,
        DataFormat_RGBA = GL_RGBA,
        DataFormat_BGRA = GL_BGRA,
        DataFormat_Red_int = GL_RED_INTEGER,
        DataFormat_RG_int = GL_RG_INTEGER,
        DataFormat_RGB_int = GL_RGB_INTEGER,
        DataFormat_BGR_int = GL_BGR_INTEGER,
        DataFormat_RGBA_int = GL_RGBA_INTEGER,
        DataFormat_BGRA_int = GL_BGRA_INTEGER,
        DataFormat_StencilIndex = GL_STENCIL_INDEX,
        DataFormat_Depth = GL_DEPTH_COMPONENT,
        DataFormat_DepthStencil = GL_DEPTH_STENCIL
    };

    enum PixelDataType {
        DataType_UByte = GL_UNSIGNED_BYTE,
        DataType_Byte = GL_BYTE,
        DataType_UShort = GL_UNSIGNED_SHORT,
        DataType_Short = GL_SHORT,
        DataType_UInt = GL_UNSIGNED_INT,
        DataType_Int = GL_INT,
        DataType_Float16 = GL_HALF_FLOAT,
        DataType_Float32 = GL_FLOAT,
        DataType_UByte_3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
        DataType_UByte_2_3_3_rev = GL_UNSIGNED_BYTE_2_3_3_REV,
        DataType_UShort_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
        DataType_UShort_5_6_5_rev = GL_UNSIGNED_SHORT_5_6_5_REV,
        DataType_UShort_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
        DataType_UShort_4_4_4_4_rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
        DataType_UShort_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
        DataType_UShort_1_5_5_5_rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
        DataType_UInt_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
        DataType_UInt_8_8_8_8_rev = GL_UNSIGNED_INT_8_8_8_8_REV,
        DataType_UInt_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
        DataType_UInt_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV
    };

    class PixelData {
        void *data;

    public:
        const PixelDataFormat dataFormat;
        const PixelDataType dataType;

        const int nDim;
        const int width, height, depth;

        const void* getData() const;

        PixelData(PixelDataFormat dataFormat, PixelDataType dataType, int nDim, int width,
                  int height, int depth, void *data);

        static Str PixelDataFormatToString(PixelDataFormat);
        static Str PixelDataTypeToString(PixelDataType);
    };

} // OpenGL

#endif //STUDIOSLAB_PIXELDATA_H
