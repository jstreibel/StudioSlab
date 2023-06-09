//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_ABSFUNCTION_H
#define V_SHAPE_ABSFUNCTION_H

#include "Mappings/RtoR/Model/RtoRFunction.h"

namespace RtoR {
    class AbsFunction : public Function {
    public:
        Real operator()(Real x) const override;

        Ptr diff(int n) const override;

        [[nodiscard]] Function<Real, Real> *Clone() const override;
    };
}

#endif //V_SHAPE_ABSFUNCTION_H
