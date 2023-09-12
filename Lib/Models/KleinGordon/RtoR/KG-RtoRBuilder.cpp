//
// Created by joao on 10/18/21.
//

#include "KG-RtoRBuilder.h"

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Backend/Console/ConsoleBackend.h"

#include "Maps/RtoR/Model/RtoRDiscreteFunctionGPU.h"
#include "KG-RtoRSystemGordonGPU.h"
#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"
#include "Maps/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Maps/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "KG-RtoRSolver.h"
#include "KG-RtoRBoundaryCondition.h"

#include "Models/KleinGordon/R2toR/EquationSolver.h"

#include "Math/Numerics/Output/Format/BinarySOF.h"
#include "Math/Numerics/Output/Format/ResolutionReductionFilter.h"
#include "Math/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "SimHistory.h"
#include "Maps/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"

#define MASSLESS_WAVE_EQ        0
#define KLEIN_GORDON_POTENTIAL  1
#define SIGNUM_GORDON_POTENTIAL 2

#define DONT_REGISTER_IMMEDIATELY false

RtoR::KGBuilder::KGBuilder(Str name, Str generalDescription, bool doRegister)
: Fields::KleinGordon::KGBuilder("RtoR-" + name, generalDescription, DONT_REGISTER_IMMEDIATELY) {
    interface->addParameters({&Potential, &mass});

    if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
}

auto RtoR::KGBuilder::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->suggestFileName();

    const auto shouldOutputOpenGL = *VisualMonitor;
    const auto shouldOutputHistory = ! *noHistoryToFile;

    if(*VisualMonitor){
        auto &backend = Backend::Initialize<GLUTBackend>();

        if(*VisualMonitor_startPaused) backend.pause();
        else backend.resume();

    }
    else Backend::Initialize<ConsoleBackend>();

    const NumericConfig &p = simulationConfig.numericConfig;

    auto *outputManager = new OutputManager(simulationConfig.numericConfig);

    /********************************************************************************************/
    int fileOutputStepsInterval = -1;
    Numerics::OutputSystem::Socket *out = nullptr;
    if(shouldOutputHistory)
    {
        const Real Tf=p.gett();

        OutputFormatterBase *outputFilter = new BinarySOF;

        auto *spaceFilter = new ResolutionReductionFilter(DimensionMetaData({(unsigned)*outputResolution}));

        const auto N = (Real) p.getN();
        const Real Np = *outputResolution;
        const Real r = p.getr();
        const auto stepsInterval = PosInt(N/(Np*r));

        if(0) outputFileName += Str("-N=") + ToStr(N, 0);

        out = new OutputHistoryToFile(simulationConfig.numericConfig, stepsInterval, spaceFilter, Tf,
                                      outputFileName, outputFilter);
        fileOutputStepsInterval = out->getnSteps();
        outputManager->addOutputChannel(out);
    }
    /********************************************************************************************/


    /********************************************************************************************/
    if(shouldOutputOpenGL) {
        auto &glutBackend = Backend::GetInstanceSuper<GLUTBackend>(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.

        auto outputOpenGL = buildOpenGLOutput();

        const auto t = p.gett();
        if(t>0)
        {
            const auto L = p.getL();
            const auto Nₒᵤₜ = *outputResolution;
            const auto nₒᵤₜ = (Resolution)(Nₒᵤₜ*t/L);
            auto simHistory = new SimHistory(simulationConfig, nₒᵤₜ, Nₒᵤₜ);

            outputManager->addOutputChannel(simHistory);
            outputOpenGL->setSimulationHistory(DummyPtr(simHistory->getData()));

            if(*OpenGLMonitor_stepsPerIdleCall < 0) {
                outputOpenGL->setnSteps(simHistory->getnSteps());
                outputOpenGL->setAutoAdjust_nSteps(true);
            }
            else {
                outputOpenGL->setnSteps(*OpenGLMonitor_stepsPerIdleCall);
                outputOpenGL->setAutoAdjust_nSteps(false);
            }
        }

        glutBackend.addWindow(std::shared_ptr<Window>(outputOpenGL));
        outputManager->addOutputChannel(outputOpenGL);
    }
    else
        /* O objetivo de relacionar o numero de passos para
         * o Console Monitor com o do file output eh para que
         * ambos possam ficar sincronizados e o integrador
         * possa rodar diversos passos antes de fazer o output. */
        outputManager->addOutputChannel(
                new OutputConsoleMonitor(simulationConfig.numericConfig, fileOutputStepsInterval > 0
                                                      ? fileOutputStepsInterval*25
                                                      : int(p.getn()/40)));

    return outputManager;

}

void *RtoR::KGBuilder::newFunctionArbitrary() {
    const size_t N = simulationConfig.numericConfig.getN();
    const floatt xLeft = simulationConfig.numericConfig.getxMin();
    const floatt xRight = xLeft + simulationConfig.numericConfig.getL();

    auto laplacianType = periodicBC
            ? RtoR::DiscreteFunction::Standard1D_PeriodicBorder
            : RtoR::DiscreteFunction::Standard1D_FixedBorder;

    if(simulationConfig.dev==CPU)
        return new RtoR::DiscreteFunction_CPU(N, xLeft, xRight, laplacianType);

#if USE_CUDA
    else if(simulationConfig.dev==GPU)
        return new RtoR::DiscreteFunctionGPU(N, xLeft, xRight, laplacianType);
#endif

    throw "Error while instantiating Field: device not recognized.";
}

void *RtoR::KGBuilder::newFieldState() {
    return new RtoR::EquationState((RtoR::DiscreteFunction*)this->newFunctionArbitrary(),
                                   (RtoR::DiscreteFunction*)this->newFunctionArbitrary());
}

void *RtoR::KGBuilder::buildEquationSolver() {
    auto potential = getPotential();
    auto dphi = (RtoR::BoundaryCondition*)getBoundary();

#if USE_CUDA == true
    if(simulationConfig.dev == device::GPU) {
        return new RtoR::SystemGordonGPU(simulationConfig.numericConfig, *dphi, *potential);
    }
#endif

    return new Fields::KleinGordon::Solver<RtoR::EquationState>(simulationConfig.numericConfig, *dphi, *potential);
}

auto RtoR::KGBuilder::buildOpenGLOutput() -> RtoR::Monitor * {
    return new RtoR::Monitor(simulationConfig.numericConfig, *(KGEnergy*)getHamiltonian());
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
    auto potential = RtoR::Function::Ptr(getPotential());
    return new KGEnergy(*this, potential);
}

Core::FunctionT<Real, Real> *RtoR::KGBuilder::getPotential() const {
    if(*Potential == MASSLESS_WAVE_EQ){
        return new RtoR::NullFunction;
    }
    else if(*Potential == KLEIN_GORDON_POTENTIAL){
        if(*mass > 0)
            return new RtoR::HarmonicPotential(*mass);
        else
            return new RtoR::NullFunction;
    }
    else if(*Potential == SIGNUM_GORDON_POTENTIAL) {
        return new RtoR::AbsFunction;
    }

    throw "Unknown potential";
}

void RtoR::KGBuilder::setLaplacianPeriodicBC() {
    Log::Info() << "KGBuilder Laplacian set to PERIODIC borders." << Log::Flush;
    periodicBC = true;
}

void RtoR::KGBuilder::setLaplacianFixedBC() {
    Log::Info() << "KGBuilder Laplacian set to FIXED borders." << Log::Flush;
    periodicBC = false;
}

bool RtoR::KGBuilder::usesPeriodicBC() const { return periodicBC; }


