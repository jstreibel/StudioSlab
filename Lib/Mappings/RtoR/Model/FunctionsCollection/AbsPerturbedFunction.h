//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_ABSPERTURBEDFUNCTION_H
#define V_SHAPE_ABSPERTURBEDFUNCTION_H

#include "Mappings/RtoR/Model/RtoRFunction.h"

#define BASE_NS 2

namespace RtoR {
    class AbsPerturbedDiffFunction : public Function {
    public:
        explicit AbsPerturbedDiffFunction(PosInt Ns = BASE_NS);

        Real operator()(Real x) const override;

        Ptr diff(int n) const override;

        Function<Real, Real> *Clone() const override;

    private:
        const PosInt Ns;
    };

    class AbsPerturbedFunction : public Function {
    public:
        explicit AbsPerturbedFunction(PosInt Ns = BASE_NS);

        Real operator()(Real x) const override;

        Ptr diff(int n) const override;

        Function<Real, Real> *Clone() const override;

    public:
        Real Fold(floatt eta, const int Ns);

    private:
        const PosInt Ns;
    };
}


#endif //V_SHAPE_ABSPERTURBEDFUNCTION_H
