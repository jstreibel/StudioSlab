//
// Created by joao on 24/06/2021
// jstreibel@gmail.com
//


#include "Integrator.h"

#define NK_INCLUDE_DEFAULT_FONT
#include "../lib/nuklear.h"

#include <iostream>
#include <boost/numeric/odeint.hpp>

#define SQ(x) (x*x)

struct KubaskiKlimasQBalls {
    static const int EqCount = 4;

    struct Params {
        Real α, β, ω_1, ω_2, μ_1, μ_2, λ_1, λ_2, l_1, l_2;
    };
    const Params p;

    static inline Real sign(const Real x) {
        return x<0?-1:(x>0?1:0);
    }

    inline void RHS(const Funcs &F, Funcs &dFdr, const Real r) const {
        assert(F.size() == dFdr.size() && F.size() == EqCount);

        const auto &fp=F[0],// f'
                   &gp=F[1],// g'
                   &f=F[2], // f
                   &g=F[3]; // g

        auto &d2fdr2=dFdr[0], // f''
             &d2gdr2=dFdr[1], // g''
             &dfdr=dFdr[2], // f'
             &dgdr=dFdr[3]; // g'

        const auto invr = 1. / r;
        const auto invr2 = 1./(r * r);

        // const auto α² = α * α;

        const auto Σ1 = /*f'' + */ 2*invr*fp + SQ(p.ω_1)*f*(1-f*f)/(1+f*f) - 2*f*fp*fp/(1+f*f) - p.l_1*(p.l_1+1)*invr2*f;
        const auto Σ2 = /*g'' + */ 2*invr*gp + SQ(p.ω_2)*g*(1-g*g)/(1+g*g) - 2*g*gp*gp/(1+g*g) - p.l_2*(p.l_2+1)*invr2*g;

        d2fdr2 = -Σ1 + 0.125*p.μ_1* sqrt(1+f*f) * sign(f) + p.λ_1*p.α*0.25*pow(f*f/(1+f*f), p.α-1)*pow(g*g/(1+g*g),p.β)*f;
        d2gdr2 = -Σ2 + 0.125*p.μ_2* sqrt(1+g*g) * sign(g) + p.λ_2*p.β*0.25*pow(f*f/(1+f*f), p.α)*pow(g*g/(1+g*g),p.β-1)*g;
        dfdr = fp;
        dgdr = gp;
    }

    void operator()(const Funcs &F, Funcs &dFdr, const Real r) const {
        return RHS(F, dFdr, r);
    }
};

struct OutputHelper {
    Output *output = nullptr;
    explicit OutputHelper(Output *output) : output(output) {}
    virtual void operator()( const Funcs &F , double r ) { output->operator()(F, r); };
};

int run(Output *output, Funcs F0, Real start_x, Real end_x, Real dx)
{
    const Real α=1, β=1,
               ω_1=0, ω_2=0,
               μ_1=0, μ_2=0,
               λ_1=0, λ_2=0,
               l_1=0, l_2=0;
    const KubaskiKlimasQBalls::Params params = {α, β, ω_1, ω_2, μ_1, μ_2, λ_1, λ_2, l_1, l_2};

    KubaskiKlimasQBalls rs = {params};

    boost::numeric::odeint::integrate(rs, F0, start_x, end_x, dx, OutputHelper(output));


    return 0;
}