//
// Created by joao on 25/06/2021.
//

#ifndef MARYLOUSTRINGS_INTEGRATOR_H
#define MARYLOUSTRINGS_INTEGRATOR_H


#include <vector>


typedef long double Real;
typedef std::vector<Real> Funcs;


struct Output {
    virtual void operator()( const Funcs &F , double x ) = 0;
};


int run(Output *output, Funcs F0, Real start_r, Real end_r, Real dr);


#endif //MARYLOUSTRINGS_INTEGRATOR_H
