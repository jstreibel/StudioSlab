//
// Created by joao on 25/06/2021.
//

#ifndef MARYLOUSTRINGS_INTEGRATOR_H
#define MARYLOUSTRINGS_INTEGRATOR_H


#include <vector>


typedef long double Real;
typedef std::vector<Real> Funcs;


struct Output {
    virtual void operator()( const Funcs &x , double t ) = 0;
};


int run(Output *, Real f0, Real dgdx0, Real dhdx0, double start_time, double end_time, double dt);


#endif //MARYLOUSTRINGS_INTEGRATOR_H
