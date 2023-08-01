//
// Created by joao on 10/18/21.
//

#include "RtoRBCInterface.h"

#include "Base/Backend/GLUT/GLUTBackend.h"
#include "Base/Backend/Console/ConsoleBackend.h"

#include "Mappings/RtoR/Model/RtoRFunctionArbitraryGPU.h"
#include "Mappings/RtoR/Model/RtoRSystemGordonGPU.h"
#include "Mappings/RtoR/Model/RtoRFunctionArbitraryCPU.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Mappings/RtoR/Model/EquationSolver.h"
#include "Mappings/RtoR/Model/RtoRBoundaryCondition.h"

#include "Mappings/RtoR/View/Graphics/RtoROutGLStatistic.h"

#include "Mappings/R2toR/Model/EquationSolver.h"

#include "Phys/Numerics/Output/Format/BinarySOF.h"
#include "Phys/Numerics/Output/Format/ResolutionReductionFilter.h"
#include "Phys/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "RtoRBuilder.h"


RtoR::RtoRBCInterface::RtoRBCInterface(Str name, Str generalDescription)
: VoidBuilder("RtoR-" + name, generalDescription) { }

auto RtoR::RtoRBCInterface::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->buildFileName();

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

        outputFileName += Str("-N=") + ToStr(N, 0);

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

void *RtoR::RtoRBCInterface::newFunctionArbitrary() {
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

void *RtoR::RtoRBCInterface::newFieldState() {
    return new RtoR::EquationState((RtoR::DiscreteFunction*)this->newFunctionArbitrary(),
                                   (RtoR::DiscreteFunction*)this->newFunctionArbitrary());
}

void *RtoR::RtoRBCInterface::getEquationSolver() {
    auto thePotential = new RtoR::AbsFunction;
    auto dphi = (RtoR::BoundaryCondition*)getBoundary();

#if USE_CUDA == true
    if(dev == device::GPU) {
        //if(potential != VShape) throw "Only signum potential implemented in GPU.";

        return new RtoR::SystemGordonGPU(numericParams, *dphi, *thePotential);
    }
#endif

    return new Phys::Gordon::GordonSolverT<RtoR::EquationState>(numericParams, *dphi, *thePotential);
}

auto RtoR::RtoRBCInterface::buildOpenGLOutput() -> RtoR::Monitor * {
    return new RtoR::OutGLStatistic(numericParams);
}

auto RtoR::RtoRBCInterface::getInitialState() -> void * {
    auto &u_0 = *(RtoR::EquationState*)newFieldState();

    u_0.setPhi(RtoR::NullFunction());
    u_0.setDPhiDt(RtoR::NullFunction());

    return &u_0;
}

#define GENERATE_METHOD(METHOD, N) \
    case (N): \
    method = new METHOD<N, typename RtoR::EquationState>(solver); \
    break;

#define GENERATE_ALL(METHOD) \
        GENERATE_METHOD(StepperRK4, 1);  \
        GENERATE_METHOD(StepperRK4, 2);  \
        GENERATE_METHOD(StepperRK4, 3);  \
        GENERATE_METHOD(StepperRK4, 4);  \
        GENERATE_METHOD(StepperRK4, 5);  \
        GENERATE_METHOD(StepperRK4, 6);  \
        GENERATE_METHOD(StepperRK4, 7);  \
        GENERATE_METHOD(StepperRK4, 8);  \
        GENERATE_METHOD(StepperRK4, 9);  \
        GENERATE_METHOD(StepperRK4, 10); \
        GENERATE_METHOD(StepperRK4, 11); \
        GENERATE_METHOD(StepperRK4, 12); \
        GENERATE_METHOD(StepperRK4, 13); \
        GENERATE_METHOD(StepperRK4, 14); \
        GENERATE_METHOD(StepperRK4, 15); \
        GENERATE_METHOD(StepperRK4, 16);

auto RtoR::RtoRBCInterface::buildStepper() -> Method * {
    Method *method;

    auto &solver = *(RtoR::EquationSolver*)getEquationSolver();

    switch (dev.get_nThreads()) {
        GENERATE_ALL(StepperRK4);
        default:
            throw "Number of threads must be between 1 and 16 inclusive.";
    }

    return method;
}


