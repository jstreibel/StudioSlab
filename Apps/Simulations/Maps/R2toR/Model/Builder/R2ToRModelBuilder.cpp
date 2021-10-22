//
// Created by joao on 10/8/21.
//

#include <Apps/Simulations/Maps/R2toR/Model/R2toRFunctionArbitraryCPU.h>
#include <Apps/Simulations/Maps/R2toR/Model/R2ToRFunctionArbitraryGPU.h>
#include <Apps/Simulations/Maps/R2toR/Model/FieldState.h>
#include <Model/SystemGordon.h>
#include <Apps/Simulations/Maps/RtoR/Model/FunctionsCollection/AbsFunction.h>
#include "R2ToRModelBuilder.h"


R2toRModelBuilder::R2toRModelBuilder(NumericParams &p, Device &dev, po::variables_map &vm)
        : Allocator(p, dev, vm)
{
    // TODO verificar se aqui precisa ser POT ou apenas divisivel pelo numero de threads na CPU (ou algo do tipo).
    if(dev.getDevice()==device::GPU)
        if(!isPOT(numericParams.getN())) throw "N must be pot for GPU sim.";

    if(p.getN()%dev.get_nThreads() != 0) throw "N%nThreads must be 0.";
}


auto R2toRModelBuilder::newFunctionArbitrary() -> void * {
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

auto R2toRModelBuilder::newFieldState() -> void * {
    return new R2toR::FieldState((R2toR::FunctionArbitrary*)this->newFunctionArbitrary(),
                                 (R2toR::FunctionArbitrary*)this->newFunctionArbitrary());
}

auto R2toRModelBuilder::getSystemSolver() -> void * {
    auto *thePotential = new RtoR::AbsFunction;
    return new Base::SystemGordon<R2toR::FieldState>(*thePotential);
}
