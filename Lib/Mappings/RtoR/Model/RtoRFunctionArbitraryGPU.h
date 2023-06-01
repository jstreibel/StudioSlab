//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
#define V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH

#include "RtoRFunctionArbitrary.h"

namespace RtoR {
#if USE_CUDA == true
    class FunctionArbitraryGPU : public ArbitraryFunction {
    public:
        FunctionArbitraryGPU(PosInt N, Real xMin, Real xMax, LaplacianType laplacianType);

        /*! This function operates on the host and then transfers information to device. Therefore, it is
         * not efficient. */
        Base::ArbitraryFunction<Real,Real> &Apply(const Function &func,
                                                  Base::ArbitraryFunction<Real, Real> &out) const override;
        Base::ArbitraryFunction<Real, Real> &Set(const MyBase &func) override;
        ArbitraryFunction &Laplacian(ArbitraryFunction &outFunc) const override;
    };
#endif
}


#endif //V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
