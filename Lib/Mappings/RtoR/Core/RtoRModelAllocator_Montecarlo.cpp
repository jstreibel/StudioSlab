//
// Created by joao on 3/13/23.
//

#include "RtoRModelAllocator_Montecarlo.h"

#include "Mappings/RtoR/Model/RtoRFunctionArbitraryGPU.h"
#include "Mappings/RtoR/Model/RtoRSystemGordonGPU.h"

#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Mappings/RtoR/Model/Equation/Montecarlo-Lorentz-2ndOrder.h"


auto RtoRModelAllocator_Montecarlo::getSystemSolver() -> void * {
#if USE_CUDA == true
    if(this->getDevice().getDevice() == device::GPU)
        throw "Unsupported device (GPU) for Langeving integration";
#endif

    RtoR::Function *thePotential = new RtoR::AbsFunction;

    return new RtoR::MontecarloLangevin_2ndOrder(*thePotential);
}
