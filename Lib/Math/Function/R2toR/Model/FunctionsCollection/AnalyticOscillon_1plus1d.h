//
// Created by joao on 7/6/24.
//

#ifndef STUDIOSLAB_ANALYTICOSCILLON_1PLUS1D_H
#define STUDIOSLAB_ANALYTICOSCILLON_1PLUS1D_H

#include "Math/Function/R2toR/Model/R2toRFunction.h"

namespace Slab::Math::R2toR {

    class AnalyticOscillon_1plus1d : public R2toR::Function {

    public:
        struct OscillonParameters {
            Real x0, t0, l, v, u, alpha;
        } oscParams;

        explicit AnalyticOscillon_1plus1d(OscillonParameters);

        auto Clone() const -> Pointer<Type> override;

        void setParams(OscillonParameters);

        Real operator()(Real2D x) const override;
    };

} // Slab::Math::R2toR

#endif //STUDIOSLAB_ANALYTICOSCILLON_1PLUS1D_H
