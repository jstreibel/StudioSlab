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
        NumericFunctionGPU(UInt N, DevFloat xMin, DevFloat xMax, LaplacianType laplacianType);

        auto Clone() const -> TPointer<FunctionT> override;

        /*! This function operates on the host and then transfers information to device. Therefore, it is
         * not efficient. */
        Base::NumericFunction<DevFloat,DevFloat> &Apply(const FunctionT &func,
                                                Base::NumericFunction<DevFloat, DevFloat> &out) const override;
        Base::NumericFunction<DevFloat, DevFloat> &Set(const MyBase &func) override;

        Base::NumericFunction<DevFloat, DevFloat> &operator+=(const MyBase &func) override;

        NumericFunction &Laplacian(NumericFunction &outFunc) const override;


        Str generalName() const override;
    };
#endif
}


#endif //V_SHAPE_FUNCTIONRTORARBITRARYGPU_CUH
