//
// Created by joao on 15/07/2021.
//

#ifndef V_SHAPE_QUASISHOCKWAVE_H
#define V_SHAPE_QUASISHOCKWAVE_H


#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class QuasiShockwave : public RtoR::Function {
        auto ϕ_1L(Real x) const -> Real;
        auto ϕ_2L(Real x) const -> Real;
        auto ϕ_3L(Real x) const -> Real;

        auto ϕ_1C(Real x) const -> Real;
        auto ϕ_2C(Real x) const -> Real;

        auto ϕ_1R(Real x) const -> Real;
        auto ϕ_2R(Real x) const -> Real;
        auto ϕ_3R(Real x) const -> Real;

        Real a0, E, a, ε;

        Real t=0;
    public:
        QuasiShockwave(Real a0, Real E, Real t=0);

        void sett(Real t){ this->t = t; }
        Real operator()(Real x) const override;
    };
}


#endif //V_SHAPE_QUASISHOCKWAVE_H
