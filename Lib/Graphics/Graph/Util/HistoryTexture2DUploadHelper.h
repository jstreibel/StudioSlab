//
// Created by joao on 21/04/24.
//

#ifndef STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H
#define STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H

#include "Utils/Numbers.h"
#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Graphics/Graph/Util/FieldTextureKontraption.h"

namespace Slab::Graphics {

    using namespace Math;

    class HistoryTexture2DUploadHelper {
        typedef Slab::Pointer<const R2toR::NumericFunction> NumericFunction_Ptr;

        Pointer<FieldTextureKontraption> textureKontraption;
        NumericFunction_Ptr function;

        Count nextRow = 0;
        Real lastUpdatedTime = -1.0;
    public:
        HistoryTexture2DUploadHelper(NumericFunction_Ptr history,
                                     Pointer<FieldTextureKontraption> texture);

        void uploadUpTo(const Real time);
    };

    DefinePointers(HistoryTexture2DUploadHelper)

} // Graphics

#endif //STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H
