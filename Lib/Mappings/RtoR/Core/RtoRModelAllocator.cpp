//
// Created by joao on 10/8/21.
//

#include "RtoRModelAllocator.h"

#include "Mappings/RtoR/Model/RtoRFunctionArbitraryGPU.h"
#include "Mappings/RtoR/Model/RtoRSystemGordonGPU.h"

#include "Mappings/RtoR/Model/RtoRFunctionArbitraryCPU.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Phys/DifferentialEquations/2nd-Order/GordonSystem.h"


RtoRModelAllocator::RtoRModelAllocator() : Numerics::Allocator("ℝ ↦ ℝ general") { };

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
        return new RtoR::FunctionArbitraryCPU(N, xLeft, xRight, RtoR::DiscreteFunction::Standard1D);

#if USE_CUDA
    else if(dev==GPU)
        return new RtoR::DiscreteFunctionGPU(N, xLeft, xRight, RtoR::DiscreteFunction::Standard1D);
#endif

    throw "Error while instantiating Field: device not recognized.";
}

auto RtoRModelAllocator::newFieldState() -> void * {
    return new RtoR::FieldState((RtoR::DiscreteFunction*)this->newFunctionArbitrary(),
                                (RtoR::DiscreteFunction*)this->newFunctionArbitrary());
}

auto RtoRModelAllocator::getSystemSolver() -> void * {
#if USE_CUDA == true
    if(dev == device::GPU) {
        //if(potential != VShape) throw "Only signum potential implemented in GPU.";

        return new RtoR::SystemGordonGPU(numericParams.getN());
    }
#endif

    RtoR::Function *thePotential;
    //if(potential == V) thePotential = new RtoR::AbsFunction;
    //else if(potential == free)
        thePotential = new RtoR::NullFunction;
    //else throw "Other potentials not implemented";

    return new Phys::Gordon::GordonSystem<RtoR::FieldState>(*thePotential);
}

void RtoRModelAllocator::SetPotential(RtoRModelAllocator::Potential pot, std::vector<Real> params) {
    auto &me = Allocator::GetInstanceSuper<RtoRModelAllocator>();

    me.potential = pot;
}
