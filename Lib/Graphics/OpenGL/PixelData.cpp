//
// Created by joao on 17/09/23.
//

#include "PixelData.h"

namespace Slab::Graphics::OpenGL {

    PixelData::PixelData(const PixelDataFormat dataFormat,
                         const PixelDataType dataType,
                         const int nDim,
                         const int width,
                         const int height,
                         const int depth,
                         void *data)
    : dataFormat(dataFormat)
    , dataType(dataType)
    , nDim(nDim)
    , width(width)
    , height(height)
    , depth(depth)
    , data(data)
    {

    }

    const void *PixelData::getData() const {
        return data;
    }

    Str PixelData::PixelDataFormatToString(PixelDataFormat format) {

        switch (format) {
            case DataFormat_Red : return "GL_RED";
            case DataFormat_RG : return "GL_RG";
            case DataFormat_RGB : return "GL_RGB";
            case DataFormat_BGR : return "GL_BGR";
            case DataFormat_RGBA : return "GL_RGBA";
            case DataFormat_BGRA : return "GL_BGRA";
            case DataFormat_Red_int : return "GL_RED_INTEGER";
            case DataFormat_RG_int : return "GL_RG_INTEGER";
            case DataFormat_RGB_int : return "GL_RGB_INTEGER";
            case DataFormat_BGR_int : return "GL_BGR_INTEGER";
            case DataFormat_RGBA_int : return "GL_RGBA_INTEGER";
            case DataFormat_BGRA_int : return "GL_BGRA_INTEGER";
            case DataFormat_StencilIndex : return "GL_STENCIL_INDEX";
            case DataFormat_Depth : return "GL_DEPTH_COMPONENT";
            case DataFormat_DepthStencil : return "GL_DEPTH_STENCIL";
        }

        return "<unknown/unimplemented pixel format>";
    }

    Str PixelData::PixelDataTypeToString(PixelDataType type) {

        switch (type) {
            case DataType_UByte : return "GL_UNSIGNED_BYTE";
            case DataType_Byte : return "GL_BYTE";
            case DataType_UShort : return "GL_UNSIGNED_SHORT";
            case DataType_Short : return "GL_SHORT";
            case DataType_UInt : return "GL_UNSIGNED_INT";
            case DataType_Int : return "GL_INT";
            case DataType_Float16 : return "GL_HALF_FLOAT";
            case DataType_Float32 : return "GL_FLOAT";
            case DataType_UByte_3_3_2 : return "GL_UNSIGNED_BYTE_3_3_2";
            case DataType_UByte_2_3_3_rev : return "GL_UNSIGNED_BYTE_2_3_3_REV";
            case DataType_UShort_5_6_5 : return "GL_UNSIGNED_SHORT_5_6_5";
            case DataType_UShort_5_6_5_rev : return "GL_UNSIGNED_SHORT_5_6_5_REV";
            case DataType_UShort_4_4_4_4 : return "GL_UNSIGNED_SHORT_4_4_4_4";
            case DataType_UShort_4_4_4_4_rev : return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
            case DataType_UShort_5_5_5_1 : return "GL_UNSIGNED_SHORT_5_5_5_1";
            case DataType_UShort_1_5_5_5_rev : return "GL_UNSIGNED_SHORT_1_5_5_5_REV";
            case DataType_UInt_8_8_8_8 : return "GL_UNSIGNED_INT_8_8_8_8";
            case DataType_UInt_8_8_8_8_rev : return "GL_UNSIGNED_INT_8_8_8_8_REV";
            case DataType_UInt_10_10_10_2 : return "GL_UNSIGNED_INT_10_10_10_2";
            case DataType_UInt_2_10_10_10_rev : return "GL_UNSIGNED_INT_2_10_10_10_REV";
        }

        return "<unknown/unimplemented pixel data type>";
    }
} // OpenGL