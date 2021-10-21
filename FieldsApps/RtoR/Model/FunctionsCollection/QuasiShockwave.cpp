//
// Created by joao on 15/07/2021.
//

#include "QuasiShockwave.h"


using namespace RtoR;


QuasiShockwave::QuasiShockwave(double a0, double E, double t) : a0(a0), E(E), a(2*a0), ε(a*a/(3*E)), t(t){ }

auto QuasiShockwave::ϕ_1L(double x) const -> double {
    auto domain = ((x > -ε + t) && (x < -t) && (t < ε / 2)) * 1;

    auto ϕ = a / (ε*ε) * (x + ε) * t - t*t / 2;

    return ϕ;
}

auto QuasiShockwave::ϕ_2L(double x) const -> double {
    auto v = -1 + ε*ε / a;

    auto arg = (x + ε - v * t);
    auto ϕ = arg*arg / (2 * (1 - v*v));

    return ϕ;
}

auto QuasiShockwave::ϕ_3L(double x) const -> double {
    auto v = -1 + ε*ε/a;

    auto arg1 = x + t;
    auto arg2 = x - t;
    auto arg3 = (ε + (1 + v) * (x - t) * .5);

    auto ϕ = 1. / (1 + v) * (ε / 2 * (x + t) - .25 * arg1*arg1) + arg2*arg2 / 8 - .5 * t*t + .5 / (1 - v*v) * arg3*arg3;

    return ϕ;
}


auto QuasiShockwave::ϕ_1C(double x) const -> double {
    auto v = -1 + ε*ε/a;

    auto ϕ = 1. / (1 + v) * (ε * t - (x*x + t*t) / 2) - t*t / 2;

    return ϕ;
}

auto QuasiShockwave::ϕ_2C(double x) const -> double {
    auto v = -1 + ε*ε / a;
    auto b = -v * t*t + 2 * ε * t - 2 * a;

    auto ϕ = (x*x - b) * .5 / (1 - v);

    // TODO consertar essa gambiarra abaixo
    if(ϕ<0) return 0;

    return ϕ;
}



auto QuasiShockwave::ϕ_1R(double x) const -> double {
    return ϕ_1L(-x);
}

auto QuasiShockwave::ϕ_2R(double x) const -> double {
    return ϕ_2L(-x);
}

auto QuasiShockwave::ϕ_3R(double x) const -> double {
    return ϕ_3L(-x);
}

Real QuasiShockwave::operator()(Real x) const {
    if((x > -ε + t) && (x < -t) && (t < ε / 2))
        return ϕ_1L(x);


    auto v = -1 + ε*ε / a;

    auto x_max = -ε + t;
    if(t > ε / 2)
        x_max = -t;
    const auto t_c = a / ε;
    if((x >= -ε + v * t) && (x <= x_max) && (t <= t_c))
        return ϕ_2L(x);


    x_max = -ε + t;
    if(t >= ε) x_max = -x_max;
    if((x >= -t) && (x <= x_max) && (t >= ε / 2))
        return ϕ_3L(x);


    if(t <= ε / 2) {
        if ((x >= -t) & (x <= t))
            return ϕ_1C(x);
    }
    else if(t > ε / 2 && t <= ε) {
        if((x >= -ε + t) & (x <= ε - t))
            return ϕ_1C(x);
    }


    if(t >= ε) if((x >= ε - t) & (x <= -ε + t))
        return ϕ_2C(x);


    if((-x > -ε + t) && (-x < -t) && (t < ε / 2))
        return ϕ_1R(x);


    x_max = -ε + t;
    if(t > ε / 2)
        x_max = -t;
    if((-x >= -ε + v * t) && (-x <= x_max) && (t <= t_c))
        return ϕ_2R(x);


    x_max = -ε + t;
    if(t >= ε) x_max = -x_max;
    if((-x >= -t) && (-x <= x_max) && (t >= ε / 2))
        return ϕ_3R(x);


    return 0;
}

