//
// Created by joao on 7/9/24.
//

#include "FromR2toRNumeric.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

namespace Slab::Math::RtoR {
    Pointer<RtoR::NumericFunction> FromR2toR(Pointer<R2toR::NumericFunction> func, Index j) {
        if(j > func->getM()-1) return nullptr;

        fix x_min = func->getDomain().xMin;
        fix x_max = func->getDomain().xMax;
        fix N = func->getN();
        auto slice = New<RtoR::NumericFunction_CPU>(N, x_min, x_max);

        OUT slice_data = slice->getSpace().getHostData();
        fix in_data = &func->At(0, j);

        for(auto i=0; i<N; ++i) slice_data[i] = in_data[i];

        return slice;
    }

    Pointer<RtoR::NumericFunction> FromR2toRAt(Pointer<R2toR::NumericFunction> func, Real t) {
        if(t > func->getDomain().yMax) return nullptr;

        fix dt = func->getDomain().getLy() / (func->getM()-1);
        fix j = t/dt;

        return FromR2toR(func, j);
    }


}
