//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_ABSPERTURBEDFUNCTION_H
#define V_SHAPE_ABSPERTURBEDFUNCTION_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

#define BASE_NS 2

namespace Slab::Math::RtoR {
    class AbsPerturbedDiffFunction : public RtoR::Function {
    public:
        explicit AbsPerturbedDiffFunction(UInt Ns = BASE_NS);

        Real operator()(Real x) const override;

        RtoR::Function_ptr diff(int n) const override;

        Pointer<FunctionT<Real, Real>> Clone() const override;

    private:
        const UInt Ns;
    };

    class AbsPerturbedFunction : public RtoR::Function {
    public:
        explicit AbsPerturbedFunction(UInt Ns = BASE_NS);

        Real operator()(Real x) const override;

        RtoR::Function_ptr diff(int n) const override;

        Pointer<FunctionT<Real, Real>> Clone() const override;

    public:
        Real Fold(floatt eta, const int Ns);

    private:
        const UInt Ns;
    };
}


#endif //V_SHAPE_ABSPERTURBEDFUNCTION_H
