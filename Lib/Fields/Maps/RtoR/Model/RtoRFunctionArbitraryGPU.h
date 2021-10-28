//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
#define V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH

#include "RtoRFunctionArbitrary.h"
#include "Studios/CoreMath/Util/device-config.h"

namespace RtoR {
#if USE_CUDA == true
    class FunctionArbitraryGPU : public FunctionArbitrary {
    public:
        FunctionArbitraryGPU(PosInt N, Real xMin, Real xMax, LaplacianType laplacianType);

        /*! This function operates on the host and then transfers information to device. It's not efficient. */
        Base::FunctionArbitrary<Real,Real> &Apply(const Function &func, Base::FunctionArbitrary<Real, Real> &out) const override;
        Base::FunctionArbitrary<Real, Real> &Set(const MyBase &func) override;
        FunctionArbitrary &Laplacian(FunctionArbitrary &outFunc) const override;
    };
#endif
}


#endif //V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
