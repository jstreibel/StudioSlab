//
// Created by joao on 18/12/2019.
//

#ifndef V_SHAPE_R2TORREGULARDELTA_H
#define V_SHAPE_R2TORREGULARDELTA_H

#include "Math/Function/Maps/R2toR/Model/R2toRFunction.h"

namespace R2toR {
    struct R2toRRegularDelta : public R2toR::Function {
    private:
        Real eps, a;
    public:
        explicit R2toRRegularDelta(Real epsilon = 0.1, Real a = 1.) : eps(epsilon), a(a) {}

        Real operator()(Real2D x) const override;
    };
}


#endif //V_SHAPE_R2TORREGULARDELTA_H
