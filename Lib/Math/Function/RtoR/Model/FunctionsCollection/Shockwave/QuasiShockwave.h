//
// Created by joao on 15/07/2021.
//

#ifndef V_SHAPE_QUASISHOCKWAVE_H
#define V_SHAPE_QUASISHOCKWAVE_H


#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class QuasiShockwave : public RtoR::Function {
        auto ϕ_1L(DevFloat x) const -> DevFloat;
        auto ϕ_2L(DevFloat x) const -> DevFloat;
        auto ϕ_3L(DevFloat x) const -> DevFloat;

        auto ϕ_1C(DevFloat x) const -> DevFloat;
        auto ϕ_2C(DevFloat x) const -> DevFloat;

        auto ϕ_1R(DevFloat x) const -> DevFloat;
        auto ϕ_2R(DevFloat x) const -> DevFloat;
        auto ϕ_3R(DevFloat x) const -> DevFloat;

        DevFloat a0, E, a, ε;

        DevFloat t=0;
    public:
        QuasiShockwave(DevFloat a0, DevFloat E, DevFloat t=0);

        void sett(DevFloat t){ this->t = t; }
        DevFloat operator()(DevFloat x) const override;
    };
}


#endif //V_SHAPE_QUASISHOCKWAVE_H
