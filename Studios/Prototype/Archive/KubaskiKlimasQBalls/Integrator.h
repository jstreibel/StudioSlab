//
// Created by joao on 25/06/2021.
//

#ifndef MARYLOUSTRINGS_INTEGRATOR_H
#define MARYLOUSTRINGS_INTEGRATOR_H


#include <vector>


typedef long double DevFloat;
typedef Vector<Real> Funcs;


struct Output {
    virtual void operator()( const Funcs &F , double x ) = 0;
};


int run(Output *output, Funcs F0, DevFloat start_r, DevFloat end_r, DevFloat dr);


#endif //MARYLOUSTRINGS_INTEGRATOR_H
