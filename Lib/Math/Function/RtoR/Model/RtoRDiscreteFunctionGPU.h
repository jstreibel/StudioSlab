//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
#define V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH

#include "RtoRDiscreteFunction.h"

namespace Slab::Math::RtoR {
#if USE_CUDA == true
    class DiscreteFunctionGPU : public DiscreteFunction {
    public:
        Str myName() const override;

        DiscreteFunctionGPU(UInt N, Real xMin, Real xMax, LaplacianType laplacianType);

        auto Clone() const -> FunctionT * override;

        /*! This function operates on the host and then transfers information to device. Therefore, it is
         * not efficient. */
        Core::DiscreteFunction<Real,Real> &Apply(const FunctionT &func,
                                                 Core::DiscreteFunction<Real, Real> &out) const override;
        Core::DiscreteFunction<Real, Real> &Set(const MyBase &func) override;
        DiscreteFunction &Laplacian(DiscreteFunction &outFunc) const override;
    };
#endif
}


#endif //V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
