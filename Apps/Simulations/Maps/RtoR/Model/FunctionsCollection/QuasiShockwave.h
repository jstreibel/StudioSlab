//
// Created by joao on 15/07/2021.
//

#ifndef V_SHAPE_QUASISHOCKWAVE_H
#define V_SHAPE_QUASISHOCKWAVE_H


#include <Apps/Simulations/Maps/RtoR/Model/RtoRFunction.h>

namespace RtoR {
    class QuasiShockwave : public Function {
        auto ϕ_1L(double x) const -> double;
        auto ϕ_2L(double x) const -> double;
        auto ϕ_3L(double x) const -> double;

        auto ϕ_1C(double x) const -> double;
        auto ϕ_2C(double x) const -> double;

        auto ϕ_1R(double x) const -> double;
        auto ϕ_2R(double x) const -> double;
        auto ϕ_3R(double x) const -> double;

        double a0, E, a, ε;

        double t=0;
    public:
        QuasiShockwave(double a0, double E, double t=0);

        void sett(double t){ this->t = t; }
        Real operator()(Real x) const override;
    };
}


#endif //V_SHAPE_QUASISHOCKWAVE_H
