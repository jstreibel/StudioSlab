//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_KG_RTORSOLVER_H
#define STUDIOSLAB_KG_RTORSOLVER_H

#include "KG-RtoREquationState.h"
#include "Math/DifferentialEquations/EquationSolver.h"
#include "Models/KleinGordon/KGSolver.h"

namespace Slab::Models::KGRtoR {
    typedef Models::Solver<EquationState> KGSolver;
}

#endif //STUDIOSLAB_KG_RTORSOLVER_H
