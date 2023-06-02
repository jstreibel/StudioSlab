//
// Created by joao on 15/10/2019.
//

#ifndef V_SHAPE_PARAMETRICCURVE_H
#define V_SHAPE_PARAMETRICCURVE_H

#include "Phys/Function/Function.h"
#include "Phys/Space/Impl/PointSet.h"

#include <memory>

namespace RtoR2 {
    class ParametricCurve : public Base::Function<Real, Real2D> {
    public:
        typedef std::shared_ptr<ParametricCurve> Ptr;

    };
}


#endif //V_SHAPE_PARAMETRICCURVE_H
