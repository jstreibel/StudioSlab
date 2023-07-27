//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_EQUATIONSOLVER_H
#define STUDIOSLAB_EQUATIONSOLVER_H

#include "Mappings/R2toR/Model/EquationState.h"
#include "Phys/DifferentialEquations/EquationSolver.h"

namespace R2toR {
    typedef Slab::EquationSolverT<R2toR::EquationState> EquationSolver;
}

#endif //STUDIOSLAB_EQUATIONSOLVER_H
