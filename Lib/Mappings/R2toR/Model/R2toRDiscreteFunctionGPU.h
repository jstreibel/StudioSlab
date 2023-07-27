//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H

#include "R2toRDiscreteFunction.h"


namespace R2toR {

    class FunctionArbitraryGPU : public R2toR::DiscreteFunction {
        FunctionArbitraryGPU *helper= nullptr;

    public:
        FunctionArbitraryGPU(PosInt N, Real sMin, Real h);
        ~FunctionArbitraryGPU();

        auto Laplacian        (DiscreteFunction &outFunc)            const -> DiscreteFunction & override;

        auto Clone()                                                 const -> Type             * override;
        auto CloneWithSize    (PosInt N)                             const -> DiscrBase        * override;

        auto Set              (const MyBase &func)                         -> DiscreteFunction & override;
        auto SetArb           (const DiscreteFunctionBase &func)           -> DiscreteFunction & override;

        auto Apply(const FuncBase &func, DiscrBase &out)             const -> DiscrBase        & override;

        auto Add              (const DiscrBase & toi)                      -> DiscreteFunction & override;
        auto StoreAddition    (const DiscrBase &, const DiscrBase&)        -> DiscreteFunction & override;
        auto StoreSubtraction (const DiscrBase &, const DiscrBase&)        -> DiscreteFunction & override;
        auto Multiply         (floatt a)                                   -> DiscreteFunction & override;

        auto operator+=(const MyBase &func)                                -> DiscrBase        & override;

        auto At               (PosInt n, PosInt m)                   const -> Real               override;
        auto At               (PosInt n, PosInt m)                         -> Real             & override;

    };

}

#endif //V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
