//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_KG_RTORSOLVER_H
#define STUDIOSLAB_KG_RTORSOLVER_H

#include "KG-RtoREquationState.h"
#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"
#include "Models/KleinGordon/KG-Solver.h"

namespace Slab::Models::KGRtoR {
    using KGRtoRSolver = Models::KGSolver<DevFloat>;
}

#endif //STUDIOSLAB_KG_RTORSOLVER_H
