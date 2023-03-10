//
// Created by joao on 10/8/21.
//

#include "RtoRModelAllocator.h"

#include "../Model/RtoRFunctionArbitraryGPU.h"
#include "../Model/RtoRSystemGordonGPU.h"

#include "../Model/RtoRFunctionArbitraryCPU.h"
#include "../Model/FunctionsCollection/AbsFunction.h"

#include <Phys/DifferentialEquations/2nd-Order/Lorentz-2ndOrder.h>


RtoRModelAllocator::RtoRModelAllocator() = default;


auto RtoRModelAllocator::Choose() -> RtoRModelAllocator* {
    auto *me = new RtoRModelAllocator;

    Allocator::Instantiate(me);

    return me;
}

//void RtoRModelAllocator::Init() {
//    auto &me = dynamic_cast<RtoRModelAllocator&>(RtoRModelAllocator::getInstance());
//
//    // me.potential = vm.count("pert") ? VShapePerturbed : VShape;
//
//    // TODO verificar se aqui precisa ser POT ou apenas divisivel pelo numero de threads na CPU (ou algo do tipo).
//    if(me.dev.getDevice()==device::GPU){
//        if(!isPOT(me.numericParams.getN())) throw "N must be pot for GPU sim.";
//    }
//
//    if(me.numericParams.getN()%me.dev.get_nThreads() != 0) throw "N%nThreads must be 0.";
//}

auto RtoRModelAllocator::newFunctionArbitrary() -> void * {
    const size_t N = numericParams.getN();
    const floatt xLeft = numericParams.getxLeft();
    const floatt xRight = xLeft + numericParams.getL();

    if(dev==CPU)
        return new RtoR::FunctionArbitraryCPU(N, xLeft, xRight, RtoR::ArbitraryFunction::Standard1D);

#if USE_CUDA
    else if(dev==GPU)
        return new RtoR::FunctionArbitraryGPU(N, xLeft, xRight, RtoR::ArbitraryFunction::Standard1D);
#endif

    throw "Error while instantiating Field: device not recognized.";
}

auto RtoRModelAllocator::newFieldState() -> void * {
    return new RtoR::FieldState((RtoR::ArbitraryFunction*)this->newFunctionArbitrary(),
                                (RtoR::ArbitraryFunction*)this->newFunctionArbitrary());
}

auto RtoRModelAllocator::getSystemSolver() -> void * {
#if USE_CUDA == true
    if(dev == device::GPU) {
        //if(potential != VShape) throw "Only signum potential implemented in GPU.";

        return new RtoR::SystemGordonGPU(numericParams.getN());
    }
#endif

    RtoR::Function *thePotential = new RtoR::AbsFunction;

    return new Base::Lorentz_2ndOrder<RtoR::FieldState>(*thePotential);
}