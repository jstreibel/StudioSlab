//
// Created by joao on 21/04/24.
//

#ifndef STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H
#define STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H

#include "Utils/Numbers.h"
#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Graphics/Plot2D/Util/FieldTextureKontraption.h"

namespace Slab::Graphics {

    using namespace Math;

    class HistoryTexture2DUploadHelper {
        typedef Slab::TPointer<const R2toR::FNumericFunction> NumericFunction_Ptr;

        TPointer<FieldTextureKontraption> textureKontraption;
        NumericFunction_Ptr function;

        CountType nextRow = 0;
        DevFloat lastUpdatedTime = -1.0;
    public:
        HistoryTexture2DUploadHelper(NumericFunction_Ptr history,
                                     TPointer<FieldTextureKontraption> texture);

        void uploadUpTo(const DevFloat time);
    };

    DefinePointers(HistoryTexture2DUploadHelper)

} // Graphics

#endif //STUDIOSLAB_HISTORYTEXTURE2DUPLOADHELPER_H
