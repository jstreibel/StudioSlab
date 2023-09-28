//
// Created by joao on 10/18/21.
//

#include "KG-RtoRBuilder.h"

#include <utility>

#include "Core/Backend/BackendManager.h"

#include "KG-RtoRSolver.h"
#include "KG-RtoRSystemGordonGPU.h"
#include "KG-RtoRBoundaryCondition.h"

#include "Math/Numerics/Output/Format/BinarySOF.h"
#include "Math/Numerics/Output/Format/ResolutionReductionFilter.h"
#include "Math/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"

#include "Maps/RtoR/Model/RtoRDiscreteFunctionGPU.h"
#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"
#include "Maps/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Maps/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Maps/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"

#include "Models/KleinGordon/R2toR/EquationSolver.h"

#include "Output/SimHistory.h"
#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"
#include "Models/KleinGordon/RtoR/Output/SnapshotOutput.h"
#include "Models/KleinGordon/RtoR/Output/DFTSnapshotOutput.h"
#include "Core/Backend/GLFW/GLFWBackend.h"

#define MASSLESS_WAVE_EQ        0
#define KLEIN_GORDON_POTENTIAL  1
#define SIGNUM_GORDON_POTENTIAL 2

#define DONT_REGISTER_IMMEDIATELY false

RtoR::KGBuilder::KGBuilder(const Str& name, Str generalDescription, bool doRegister)
: Fields::KleinGordon::KGBuilder("RtoR-" + name, std::move(generalDescription),
                                 DONT_REGISTER_IMMEDIATELY)
{
    interface->addParameters({&Potential, &mass});

    if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
}

auto RtoR::KGBuilder::buildOutputManager() -> OutputManager * {
    StrUtils::UseScientificNotation = false;
    StrUtils::RealToStringDecimalPlaces = 7;
    
    auto outputFileName = Common::GetPWD() + "/" + this->suggestFileName();

    const auto shouldOutputOpenGL = *VisualMonitor;
    const auto shouldOutputHistory = ! *noHistoryToFile;

    if(*VisualMonitor){
        Core::BackendManager::Startup(Core::GLFW);
        // Core::BackendManager::Startup(Core::GLUT);
        auto &backend = Core::BackendManager::GetGUIBackend();

        if(*VisualMonitor_startPaused) backend.pause();
        else backend.resume();
    }
    else Core::BackendManager::Startup(Core::Headless);

    const NumericConfig &p = simulationConfig.numericConfig;

    auto *outputManager = new OutputManager(simulationConfig.numericConfig);

    fix t = p.gett();
    fix N = (Real) p.getN();
    fix L = p.getL();
    fix xMin = p.getxMin();
    fix Nₒᵤₜ = *outputResolution > N ? N : *outputResolution;
    fix r = p.getr();



    /* ****************************************************************************************
       *************************** SNAPSHOT OUTPUT ********************************************
       **************************************************************************************** */
    if(*takeSnapshot) {
        auto snapshotFilename = Common::GetPWD() + "/snapshots/" + suggestFileName();
        outputManager->addOutputChannel(
                new KleinGordon::RtoR::SnapshotOutput(simulationConfig.numericConfig, snapshotFilename));
    }
    if(*takeDFTSnapshot) {
        auto snapshotFilename = Common::GetPWD() + "/snapshots/" + suggestFileName();
        outputManager->addOutputChannel(
                new KleinGordon::RtoR::DFTSnapshotOutput(simulationConfig.numericConfig, snapshotFilename));
    }



    /* ****************************************************************************************
       *************************** HISTORY OUTPUT *********************************************
       **************************************************************************************** */
    int fileOutputStepsInterval = -1;
    Numerics::OutputSystem::Socket *out = nullptr;
    if(shouldOutputHistory)
    {
        OutputFormatterBase *outputFilter = new BinarySOF;

        auto *spaceFilter = new ResolutionReductionFilter(DimensionMetaData({(unsigned)*outputResolution}, {L/ *outputResolution}));

        fix stepsInterval = UInt(N/(Nₒᵤₜ*r));

        out = new OutputHistoryToFile(simulationConfig.numericConfig, stepsInterval, spaceFilter, t,
                                      outputFileName, outputFilter);
        fileOutputStepsInterval = out->getnSteps();
        outputManager->addOutputChannel(out);
    }



    /* ****************************************************************************************
       *************************** VISUAL MONITOR *********************************************
       **************************************************************************************** */
    if(shouldOutputOpenGL) {
        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        auto outputOpenGL = buildOpenGLOutput();

        if(t>0)
        {
            fix nₒᵤₜ = ( (Nₒᵤₜ/L) * t );

            auto simHistory = new SimHistory(simulationConfig,
                                             (Resolution)Nₒᵤₜ,
                                             (Resolution)nₒᵤₜ,
                                             xMin,
                                             L);
            //auto ftHistory = new SimHistory_FourierTransform(simulationConfig, 1000, 0, 40*M_PI);
            auto ftHistory = new SimHistory_DFT(simulationConfig, nₒᵤₜ);

            outputManager->addOutputChannel(simHistory);
            outputManager->addOutputChannel(ftHistory);

            outputOpenGL->setSimulationHistory   (DummyPtr(simHistory->getData()));
            outputOpenGL->setSpaceFourierHistory (DummyPtr(ftHistory ->getData()));

            if(*OpenGLMonitor_stepsPerIdleCall < 0) {
                outputOpenGL->setnSteps(simHistory->getnSteps());
                outputOpenGL->setAutoAdjust_nSteps(true);
            }
            else {
                outputOpenGL->setnSteps(*OpenGLMonitor_stepsPerIdleCall);
                outputOpenGL->setAutoAdjust_nSteps(false);
            }
        }

        guiBackend.addEventListener(std::shared_ptr<Window>(outputOpenGL));
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

Str RtoR::KGBuilder::suggestFileName() const {
    auto strParams = interface->toString({"mass"}, " ");
    Log::Debug() << strParams << Log::Flush;
    auto voidSuggestion = VoidBuilder::suggestFileName();
    return voidSuggestion + " " + strParams;
}


