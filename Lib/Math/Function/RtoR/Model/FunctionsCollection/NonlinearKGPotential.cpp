//
// Created by joao on 7/23/24.
//

#include "NonlinearKGPotential.h"

#include <cmath>

namespace Slab::Math::RtoR {

    Real128 lambda(int n, int N) {
        auto log_gamma = lgammaf128;

        fix nu = (Real)n / (Real)N;

        fix t1 = n*log(2.);
        fix t2 = log(2./(M_PI*n*n));
        fix t3 = log((1.+nu)/(1.-nu));
        fix t4 = -log_gamma(n);
        fix t5 = -log_gamma(.5*(N-n));
        fix t6 =  log_gamma(.5*(N+n));

        fix exponent = t1+t2+t3+t4+t5+t6;

        return expf128(Real128(.5*(Real)N)*exponent);
    }

} // Slab::Math::RtoR