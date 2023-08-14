//
// Created by joao on 10/18/21.
//

#include "KG-RtoRBuilder.h"

#include "Base/Backend/GLUT/GLUTBackend.h"
#include "Base/Backend/Console/ConsoleBackend.h"

#include "Mappings/RtoR/Model/RtoRDiscreteFunctionGPU.h"
#include "KG-RtoRSystemGordonGPU.h"
#include "Mappings/RtoR/Model/RtoRDiscreteFunctionCPU.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "KG-RtoRSolver.h"
#include "KG-RtoRBoundaryCondition.h"

#include "Models/KleinGordon/R2toR/EquationSolver.h"

#include "Phys/Numerics/Output/Format/BinarySOF.h"
#include "Phys/Numerics/Output/Format/ResolutionReductionFilter.h"
#include "Phys/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"


RtoR::KGBuilder::KGBuilder(Str name, Str generalDescription, bool doRegister)
: Fields::KleinGordon::KGBuilder("RtoR-" + name, generalDescription, doRegister) { }

auto RtoR::KGBuilder::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->suggestFileName();

    const auto shouldOutputOpenGL = *VisualMonitor;
    const auto shouldOutputHistory = ! *noHistoryToFile;

    if(*VisualMonitor) Backend::Initialize<GLUTBackend>();
    else Backend::Initialize<ConsoleBackend>();

    const NumericParams &p = numericParams;

    auto *outputManager = new OutputManager(numericParams);

    /********************************************************************************************/
    int fileOutputStepsInterval = -1;
    if(shouldOutputHistory)
    {
        const Real Tf=p.gett();

        OutputFormatterBase *outputFilter = new BinarySOF;
        //OutputFormatterBase *outputFilter = new CustomStringSeparatedSOF;

        auto *spaceFilter = new ResolutionReductionFilter(DimensionMetaData({(unsigned)*outputResolution}));

        const auto N = (Real) p.getN();
        const Real Np = *outputResolution;
        const Real r = p.getr();
        const auto stepsInterval = PosInt(N/(Np*r));

        if(0) outputFileName += Str("-N=") + ToStr(N, 0);

        Numerics::OutputSystem::Socket *out = new OutputHistoryToFile(numericParams, stepsInterval, spaceFilter, Tf,
                                                                      outputFileName, outputFilter);
        fileOutputStepsInterval = out->getnSteps();
        outputManager->addOutputChannel(out);
    }
    /********************************************************************************************/


    /********************************************************************************************/
    if(shouldOutputOpenGL) {
        auto &glutBackend = Backend::GetInstanceSuper<GLUTBackend>(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.

        //Base::OutputOpenGL *outputOpenGL = new RtoR::OutputOpenGL(xLeft, xRight, phiMin, phiMax);
        auto outputOpenGL = buildOpenGLOutput();
        glutBackend.addWindow(std::shared_ptr<Window>(outputOpenGL));
        // outGL->output(dummyInfo); // stop flicker?
        outputManager->addOutputChannel(outputOpenGL);
    }
    else
        /* O objetivo de relacionar o numero de passos para
         * o Console Monitor com o do file output eh para que
         * ambos possam ficar sincronizados e o integrador
         * possa rodar diversos passos antes de fazer o output. */
        outputManager->addOutputChannel(
                new OutputConsoleMonitor(numericParams, fileOutputStepsInterval>0
                                                        ? fileOutputStepsInterval*25
                                                        : int(p.getn()/40)));

    return outputManager;

}

void *RtoR::KGBuilder::newFunctionArbitrary() {
    const size_t N = numericParams.getN();
    const floatt xLeft = numericParams.getxMin();
    const floatt xRight = xLeft + numericParams.getL();

    if(dev==CPU)
        return new RtoR::FunctionArbitraryCPU(N, xLeft, xRight, RtoR::DiscreteFunction::Standard1D);

#if USE_CUDA
    else if(dev==GPU)
        return new RtoR::DiscreteFunctionGPU(N, xLeft, xRight, RtoR::DiscreteFunction::Standard1D);
#endif

    throw "Error while instantiating Field: device not recognized.";
}

void *RtoR::KGBuilder::newFieldState() {
    return new RtoR::EquationState((RtoR::DiscreteFunction*)this->newFunctionArbitrary(),
                                   (RtoR::DiscreteFunction*)this->newFunctionArbitrary());
}

void *RtoR::KGBuilder::buildEquationSolver() {
    auto thePotential = new RtoR::AbsFunction;
    auto dphi = (RtoR::BoundaryCondition*)getBoundary();

#if USE_CUDA == true
    if(dev == device::GPU) {
        //if(potential != VShape) throw "Only signum potential implemented in GPU.";

        return new RtoR::SystemGordonGPU(numericParams, *dphi, *thePotential);
    }
#endif

    return new Fields::KleinGordon::Solver<RtoR::EquationState>(numericParams, *dphi, *thePotential);
}

auto RtoR::KGBuilder::buildOpenGLOutput() -> RtoR::Monitor * {
    //return new RtoR::OutGLStatistic(numericParams);
    return new RtoR::Monitor(numericParams, *(KGEnergy*)getHamiltonian());
}

auto RtoR::KGBuilder::getInitialState() -> void * {
    auto &u_0 = *(RtoR::EquationState*)newFieldState();

    u_0.setPhi(RtoR::NullFunction());
    u_0.setDPhiDt(RtoR::NullFunction());

    return &u_0;
}

auto RtoR::KGBuilder::buildStepper() -> Stepper * {
    auto &solver = *(RtoR::KGSolver*) buildEquationSolver();

    return new StepperRK4<typename RtoR::EquationState>(solver);
}

void *RtoR::KGBuilder::getHamiltonian() {
    return new KGEnergy(*this);
}


