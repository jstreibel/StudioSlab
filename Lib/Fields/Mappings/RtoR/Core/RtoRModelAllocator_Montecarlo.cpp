//
// Created by joao on 3/13/23.
//

#include "RtoRModelAllocator_Montecarlo.h"

#include "../Model/RtoRFunctionArbitraryGPU.h"
#include "../Model/RtoRSystemGordonGPU.h"

#include "../Model/FunctionsCollection/AbsFunction.h"

#include "Fields/Mappings/RtoR/Model/Equation/Montecarlo-Lorentz-2ndOrder.h"





auto RtoRModelAllocator_Montecarlo::Choose() -> RtoRModelAllocator_Montecarlo* {
    auto *me = new RtoRModelAllocator_Montecarlo;

    Allocator::Instantiate(me);

    return me;
}


auto RtoRModelAllocator_Montecarlo::getSystemSolver() -> void * {
#if USE_CUDA == true
    if(this->getDevice().getDevice() == device::GPU)
        throw "Unsupported device (GPU) for Langeving integration";
#endif

    RtoR::Function *thePotential = new RtoR::AbsFunction;

    return new RtoR::MontecarloLangevin_2ndOrder(*thePotential);
}
