//
// Created by joao on 21/04/24.
//

#ifndef STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H
#define STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H

#include "Utils/Numbers.h"
#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Slab::Graphics {

    using namespace Math;

    class HistoryTexture2DUploadHelper {
        typedef Slab::Pointer<OpenGL::Texture2D_Real> Texture2D_Real_Ptr;
        typedef Slab::Pointer<const R2toR::NumericFunction> NumericFunction_Ptr;

        Texture2D_Real_Ptr textureData;
        NumericFunction_Ptr function;

        Count nextRow = 0;
        Real lastUpdatedTime = -1.0;
    public:
        HistoryTexture2DUploadHelper(NumericFunction_Ptr history, Texture2D_Real_Ptr texture);

        void uploadUpTo(const Real time);
    };

    DefinePointer(HistoryTexture2DUploadHelper)

} // Graphics

#endif //STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H
