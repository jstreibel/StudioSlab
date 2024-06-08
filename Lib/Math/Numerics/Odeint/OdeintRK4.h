//
// Created by joao on 6/1/24.
//

#ifndef STUDIOSLAB_ODEINTRK4_H
#define STUDIOSLAB_ODEINTRK4_H

#include "OdeintAlgebra.h"
#include <boost/numeric/odeint.hpp>

namespace Slab::Math::Odeint {
    using StepperRK4 = boost::numeric::odeint::runge_kutta4<
            OdeintStateWrapper, double, OdeintStateWrapper, double, OdeintAlgebra>;
}

#endif //STUDIOSLAB_ODEINTRK4_H
