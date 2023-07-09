//
// Created by joao on 10/8/21.
//

#include "RtoRModelAllocator_Langevin.h"

#include "Mappings/RtoR/Model/RtoRFunctionArbitraryGPU.h"
#include "Mappings/RtoR/Model/RtoRSystemGordonGPU.h"

#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Mappings/RtoR/Model/Equation/Langevin-Lorentz-2ndOrder.h"



auto RtoRModelAllocator_Langevin::getSystemSolver() -> void * {
#if USE_CUDA == true
    if(this->getDevice().getDevice() == device::GPU)
        throw "Unsupported device (GPU) for Langeving integration";
#endif

    RtoR::Function *thePotential = new RtoR::AbsFunction;

    return new RtoR::LorentzLangevin_2ndOrder(*thePotential);
}

