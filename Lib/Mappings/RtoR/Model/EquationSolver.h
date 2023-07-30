//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_EQUATIONSOLVER_H
#define STUDIOSLAB_EQUATIONSOLVER_H

#include "Mappings/RtoR/Model/RtoRFieldState.h"
#include "Phys/DifferentialEquations/EquationSolver.h"
#include "Phys/DifferentialEquations/2nd-Order/GordonSystemT.h"

namespace RtoR {
    // typedef Slab::EquationSolverT<R2toR::EquationState> EquationSolver;
    typedef Phys::Gordon::GordonSolverT<RtoR::EquationState> EquationSolver;
}

#endif //STUDIOSLAB_EQUATIONSOLVER_H
