//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_KG_RTORSOLVER_H
#define STUDIOSLAB_KG_RTORSOLVER_H

#include "KG-RtoREquationState.h"
#include "Phys/DifferentialEquations/EquationSolver.h"
#include "Models/KleinGordon/KGSolver.h"

namespace RtoR {
    typedef Fields::KleinGordon::Solver<RtoR::EquationState> EquationSolver;
}

#endif //STUDIOSLAB_KG_RTORSOLVER_H