//
// Created by joao on 18/12/2019.
//

#ifndef V_SHAPE_R2TORREGULARDELTA_H
#define V_SHAPE_R2TORREGULARDELTA_H

#include "Math/Function/R2toR/Model/R2toRFunction.h"

namespace Slab::Math::R2toR {
    struct R2toRRegularDelta : public R2toR::Function {
    private:
        DevFloat eps, a;
    public:
        explicit R2toRRegularDelta(DevFloat epsilon = 0.1, DevFloat a = 1.) : eps(epsilon), a(a) {}

        DevFloat operator()(Real2D x) const override;
    };
}


#endif //V_SHAPE_R2TORREGULARDELTA_H
