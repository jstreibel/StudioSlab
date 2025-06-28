//
// Created by joao on 7/9/24.
//

#include "FromR2toRNumeric.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Data/DataAllocator.h"

namespace Slab::Math::RtoR {
    Pointer<RtoR::NumericFunction> FromR2toR(Pointer<const R2toR::NumericFunction> func, Index j,
                                             NumericFunction::LaplacianType laplacian_type) {
        if(j > func->getM()-1) return nullptr;

        fix x_min = func->getDomain().xMin;
        fix x_max = func->getDomain().xMax;
        fix N = func->getN();
        auto slice = DataAlloc<RtoR::NumericFunction_CPU>(func->get_data_name() + " [slice@j=" + ToStr(j) + "]", N, x_min, x_max, laplacian_type);

        OUT slice_data = slice->getSpace().getHostData();
        fix in_data = &func->At(0, j);

        for(auto i=0; i<N; ++i) slice_data[i] = in_data[i];

        return slice;
    }

    Pointer<RtoR::NumericFunction> FromR2toRAt(Pointer<const R2toR::NumericFunction> func, DevFloat t) {
        if(t > func->getDomain().yMax) return nullptr;

        fix dt = func->getDomain().getLy() / (func->getM()-1);
        fix j = t/dt;

        return FromR2toR(func, j);
    }


}
