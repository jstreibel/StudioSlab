//
// Created by joao on 6/1/24.
//

#ifndef STUDIOSLAB_ODEINTSOLVER_H
#define STUDIOSLAB_ODEINTSOLVER_H

#include "OdeintRK4.h"

#include "Math/Numerics/Solver/Solver.h"

namespace Slab::Math::Odeint {

    class OdeintSolver {
    public:
        OdeintSolver() = default;

        virtual void operator()(const OdeintStateWrapper& x, OdeintStateWrapper& dxdt, double t) = 0;
    };
}

#endif //STUDIOSLAB_ODEINTSOLVER_H
