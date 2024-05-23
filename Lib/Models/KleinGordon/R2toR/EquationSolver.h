//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_KG_RTORSOLVER_H
#define STUDIOSLAB_EQUATIONSOLVER_H

#include "EquationState.h"
#include "Math/DifferentialEquations/EquationSolver.h"
#include "Models/KleinGordon/KGSolver.h"

namespace Slab::Math::R2toR {
    // typedef Slab::EquationSolverT<R2toR::EquationState> EquationSolver;
    typedef Models::Solver<R2toR::EquationState> EquationSolver;
}

#endif //STUDIOSLAB_EQUATIONSOLVER_H
