//
// Created by joao on 10/8/21.
//

#include "R2toR_Allocator.h"

#include "Mappings/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunctionGPU.h"
#include "Mappings/R2toR/Model/EquationState.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Phys/DifferentialEquations/2nd-Order/GordonSystem.h"

R2toR::Core::BasicAllocator::BasicAllocator(String name) : Numerics::Allocator(name) {

};
//{
//    // TODO verificar se aqui precisa ser POT ou apenas divisivel pelo numero de threads na CPU (ou algo do tipo).
//    if(dev.getDevice()==device::GPU)
//        if(!isPOT(numericParams.getN())) throw "N must be pot for GPU sim.";
//
//    if(p.getN()%dev.get_nThreads() != 0) throw "N%nThreads must be 0.";
//}

auto R2toR::Core::BasicAllocator::Choose() -> R2toR::Core::BasicAllocator * {
    auto *me = new R2toR::Core::BasicAllocator;

    BasicAllocator::Instantiate(me);

    return me;
}

auto R2toR::Core::BasicAllocator::newFunctionArbitrary() -> void * {
    const size_t N = numericParams.getN();
    const floatt xLeft = numericParams.getxLeft();

    if(dev==CPU)
        return new R2toR::FunctionArbitraryCPU(N, N, xLeft, xLeft, numericParams.geth());

#if USE_CUDA
    else if(dev==GPU)
        return new R2toR::FunctionArbitraryGPU(N, xLeft, numericParams.geth());
#endif

    throw "Error while instantiating Field: device not recognized.";
}

auto R2toR::Core::BasicAllocator::newFieldState() -> void * {
    return new R2toR::EquationState((R2toR::DiscreteFunction*)this->newFunctionArbitrary(),
                                    (R2toR::DiscreteFunction*)this->newFunctionArbitrary());
}

auto R2toR::Core::BasicAllocator::getSystemSolver() -> void * {
    RtoR::Function *thePotential = new RtoR::AbsFunction;
    return new Phys::Gordon::GordonSystem<R2toR::EquationState>(*thePotential);
}


