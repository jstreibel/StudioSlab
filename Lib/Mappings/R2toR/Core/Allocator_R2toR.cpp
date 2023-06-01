//
// Created by joao on 10/8/21.
//

#include "Allocator_R2toR.h"

#include "Mappings/R2toR/Model/R2toRFunctionArbitraryCPU.h"
#include "Mappings/R2toR/Model/R2ToRFunctionArbitraryGPU.h"
#include "Mappings/R2toR/Model/FieldState.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Phys/Numerics/Program/NumericParams.h"
#include "Phys/DifferentialEquations/2nd-Order/GordonSystem.h"

Allocator_R2toR::Allocator_R2toR() = default;
//{
//    // TODO verificar se aqui precisa ser POT ou apenas divisivel pelo numero de threads na CPU (ou algo do tipo).
//    if(dev.getDevice()==device::GPU)
//        if(!isPOT(numericParams.getN())) throw "N must be pot for GPU sim.";
//
//    if(p.getN()%dev.get_nThreads() != 0) throw "N%nThreads must be 0.";
//}

auto Allocator_R2toR::Choose() -> Allocator_R2toR * {
    auto *me = new Allocator_R2toR;

    Allocator::Instantiate(me);

    return me;
}

auto Allocator_R2toR::newFunctionArbitrary() -> void * {
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

auto Allocator_R2toR::newFieldState() -> void * {
    return new R2toR::FieldState((R2toR::FunctionArbitrary*)this->newFunctionArbitrary(),
                                 (R2toR::FunctionArbitrary*)this->newFunctionArbitrary());
}

auto Allocator_R2toR::getSystemSolver() -> void * {
    RtoR::Function *thePotential = new RtoR::AbsFunction;
    return new Phys::Gordon::GordonSystem<R2toR::FieldState>(*thePotential);
}


