//
// Created by joao on 15/10/2019.
//

#ifndef V_SHAPE_PARAMETRICCURVE_H
#define V_SHAPE_PARAMETRICCURVE_H

#include "Math/Function/Function.h"
#include "Math/VectorSpace/Impl/PointSet.h"

#include <memory>

namespace Slab::Math::RtoR2 {

    class ParametricCurve : public Base::FunctionT<Real, Real2D> {
        Real sMin=.0, sMax=1.;

    public:
        typedef std::shared_ptr<ParametricCurve> Ptr;

        ParametricCurve() = default;
        ParametricCurve(Real sMin, Real sMax) : sMin(sMin), sMax(sMax) {};

        auto normalize(Real s) const -> Real {
            fix Δs = getΔs();

            return (s-sMin)/Δs;
        };

        inline Real getΔs() const {return sMax - sMin;}
        inline Real get_sMin() const {return sMin;}
        inline Real get_sMax() const {return sMax;}
        inline void set_s(Real min, Real max) { sMin = min; sMax = max; }
    };

    DefinePointers(ParametricCurve)
}


#endif //V_SHAPE_PARAMETRICCURVE_H
