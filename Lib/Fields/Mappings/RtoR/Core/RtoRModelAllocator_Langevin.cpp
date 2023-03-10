//
// Created by joao on 10/8/21.
//

#include "RtoRModelAllocator_Langevin.h"

#include "../Model/RtoRFunctionArbitraryGPU.h"
#include "../Model/RtoRSystemGordonGPU.h"

#include "../Model/FunctionsCollection/AbsFunction.h"

#include "Fields/Mappings/RtoR/Model/Equation/Langevin-Lorentz-2ndOrder.h"



RtoRModelAllocator_Langevin::RtoRModelAllocator_Langevin() {

}


auto RtoRModelAllocator_Langevin::Choose() -> RtoRModelAllocator_Langevin* {
    auto *me = new RtoRModelAllocator_Langevin;

    Allocator::Instantiate(me);

    return me;
}


auto RtoRModelAllocator_Langevin::getSystemSolver() -> void * {
#if USE_CUDA == true
    if(this->getDevice().getDevice() == device::GPU)
        throw "Unsupported device (GPU) for Langeving integration";
#endif

    RtoR::Function *thePotential = new RtoR::AbsFunction;

    return new RtoR::LorentzLangevin_2ndOrder(*thePotential);
}

