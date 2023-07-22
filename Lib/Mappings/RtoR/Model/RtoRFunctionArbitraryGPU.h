//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
#define V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH

#include "RtoRFunctionArbitrary.h"

namespace RtoR {
#if USE_CUDA == true
    class DiscreteFunctionGPU : public DiscreteFunction {
    public:
        DiscreteFunctionGPU(PosInt N, Real xMin, Real xMax, LaplacianType laplacianType);

        /*! This function operates on the host and then transfers information to device. Therefore, it is
         * not efficient. */
        Base::DiscreteFunction<Real,Real> &Apply(const FunctionT &func,
                                                 Base::DiscreteFunction<Real, Real> &out) const override;
        Base::DiscreteFunction<Real, Real> &Set(const MyBase &func) override;
        DiscreteFunction &Laplacian(DiscreteFunction &outFunc) const override;
    };
#endif
}


#endif //V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
