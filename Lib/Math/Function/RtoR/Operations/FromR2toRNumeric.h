//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_FROMR2TORNUMERIC_H
#define STUDIOSLAB_FROMR2TORNUMERIC_H

#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Slab::Math::RtoR {
    Pointer<RtoR::NumericFunction> FromR2toR(Pointer<R2toR::NumericFunction>, Index j);

    Pointer<RtoR::NumericFunction> FromR2toRAt(Pointer<R2toR::NumericFunction>, Real t);
}

#endif //STUDIOSLAB_FROMR2TORNUMERIC_H
