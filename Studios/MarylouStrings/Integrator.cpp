//
// Created by joao on 24/06/2021
// jstreibel@gmail.com
//


#include "Integrator.h"

#define NK_INCLUDE_DEFAULT_FONT
#include "../lib/nuklear.h"

#include <iostream>
#include <boost/numeric/odeint.hpp>


struct StringsOfMarylou {

    const Real beta, alpha_phi;

    inline void RHS(const Funcs &V, Funcs &dVdx, const double x) const {
        const auto &c=V[0],
                   &d=V[1],
                   &e=V[2],
                   &f=V[3],
                   &g=V[4],
                   &h=V[5];

        auto &dcdx=dVdx[0], // f''
             &dddx=dVdx[1], // g''
             &dedx=dVdx[2], // h''
             &dfdx=dVdx[3], // f'
             &dgdx=dVdx[4], // g'
             &dhdx=dVdx[5]; // h'

        const auto invx = 1./x;
        const auto a2 = alpha_phi*alpha_phi;
        const auto n2 = 1.0;
        const auto sqTerm = (beta/4)*(f*f + g*g - 2);
        dcdx = -invx*c + f*sqTerm;
        dddx = -invx*d + g*sqTerm + n2*invx*invx*g*(1-h)*(1-h);
        dedx =  invx*e - a2*(1-h)*g*g;
        dfdx = c;
        dgdx = d;
        dhdx = e;
    }

    void operator()(const Funcs &V, Funcs &dVdx, const Real x) {
        return RHS(V, dVdx, x);
    }
};


struct OutputHelper {
    Output *output = nullptr;
    explicit OutputHelper(Output *output) : output(output) {}
    virtual void operator()( const Funcs &x , double t ) { output->operator()(x, t); };
};



int run(Output *output, Real f0, Real dgdx0, Real dhdx0, double start_time, double end_time, double dt)
{
    const Real beta=0.5, alpha_phi=2;
    const Real dfdx0=0, g0=0, h0=0;  // don't touch this;
    Funcs V0 = {dfdx0, dgdx0, dhdx0,
                f0, g0, h0};
    StringsOfMarylou rs = {beta, alpha_phi};

    if(1) {
        boost::numeric::odeint::integrate(rs, V0, start_time, end_time, dt, OutputHelper(output));
    } else if(1) {
        boost::numeric::odeint::runge_kutta4<Funcs> stepper;
        boost::numeric::odeint::integrate_const( stepper , rs, V0, start_time , end_time , dt );
    }

    return 0;
}