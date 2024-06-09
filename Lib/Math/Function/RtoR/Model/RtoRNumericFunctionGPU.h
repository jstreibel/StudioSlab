//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
#define V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH

#include "RtoRNumericFunction.h"

namespace Slab::Math::RtoR {
#if USE_CUDA == true
    class NumericFunctionGPU : public NumericFunction {
    public:
        Str myName() const override;

        NumericFunctionGPU(UInt N, Real xMin, Real xMax, LaplacianType laplacianType);

        auto Clone() const -> FunctionT * override;

        /*! This function operates on the host and then transfers information to device. Therefore, it is
         * not efficient. */
        Core::NumericFunction<Real,Real> &Apply(const FunctionT &func,
                                                 Core::NumericFunction<Real, Real> &out) const override;
        Core::NumericFunction<Real, Real> &Set(const MyBase &func) override;
        NumericFunction &Laplacian(NumericFunction &outFunc) const override;
    };
#endif
}


#endif //V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
