//
// Created by joao on 10/18/21.
//

#include "KG-RtoRBuilder.h"

#include <utility>

#include "Utils/Files.h"

#include "Core/Backend/BackendManager.h"

#include "KG-RtoRSolver.h"
#include "KG-RtoRBoundaryCondition.h"

#include "Math/Numerics/Output/Format/BinarySOF.h"
#include "Math/Numerics/Output/Format/ResolutionReductionFilter.h"
#include "Math/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"

#include "Output/SimHistory.h"
#include "Output/SnapshotOutput.h"
#include "Output/DFTSnapshotOutput.h"

#include "Graphics/Graph/ℝ↦ℝ/RtoRMonitor.h"

#define MASSLESS_WAVE_EQ        0
#define KLEIN_GORDON_POTENTIAL  1
#define SIGNUM_GORDON_POTENTIAL 2

#define DONT_REGISTER_IMMEDIATELY false


namespace Slab::Math {

    RtoR::KGBuilder::KGBuilder(const Str &name, Str generalDescription, bool doRegister)
            : Models::KGBuilder("RtoR-" + name, std::move(generalDescription),
                                             DONT_REGISTER_IMMEDIATELY) {
        interface->addParameters({&Potential, &massSqr});

        if (doRegister) InterfaceManager::getInstance().registerInterface(interface);
    }

    auto RtoR::KGBuilder::buildOutputManager() -> OutputManager * {
        StrUtils::UseScientificNotation = false;
        StrUtils::RealToStringDecimalPlaces = 7;

        auto outputFileName = Common::GetPWD() + "/" + this->suggestFileName();

        const auto shouldOutputOpenGL = *VisualMonitor;
        const auto shouldOutputHistory = !*noHistoryToFile;

        if (*VisualMonitor) Core::BackendManager::Startup(Core::GLFW);
        else                Core::BackendManager::Startup(Core::Headless);

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
        if (*takeSnapshot) {
            auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            Utils::CheckFolderExists(snapshotsFolder);

            auto snapshotFilename = snapshotsFolder + suggestFileName();
            outputManager->addOutputChannel(
                    Slab::New<SnapshotOutput>(simulationConfig.numericConfig, snapshotFilename));
        }
        if (*takeDFTSnapshot) {
            auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            Utils::CheckFolderExists(snapshotsFolder);

            auto snapshotFilename = snapshotsFolder + suggestFileName();
            outputManager->addOutputChannel(
                    Slab::New<DFTSnapshotOutput>(simulationConfig.numericConfig, snapshotFilename));
        }



        /* ****************************************************************************************
           *************************** HISTORY OUTPUT *********************************************
           **************************************************************************************** */
        int fileOutputStepsInterval = -1;
        Socket_ptr out;
        if (shouldOutputHistory) {
            OutputFormatterBase *outputFilter = new BinarySOF;

            auto *spaceFilter = new ResolutionReductionFilter(
                    DimensionMetaData({(unsigned) *outputResolution}, {L / *outputResolution}));

            fix stepsInterval = UInt(N / (Nₒᵤₜ * r));

            out = Slab::New<OutputHistoryToFile>(simulationConfig.numericConfig, stepsInterval, spaceFilter, t,
                                          outputFileName, outputFilter);
            fileOutputStepsInterval = out->getnSteps();
            outputManager->addOutputChannel(out);
        }



        /* ****************************************************************************************
           *************************** VISUAL MONITOR *********************************************
           **************************************************************************************** */
        if (shouldOutputOpenGL) {
            auto &guiBackend = Core::BackendManager::GetGUIBackend();

            auto outputOpenGL = Pointer<RtoR::Monitor>((RtoR::Monitor*)buildOpenGLOutput());

            if (t > 0) {
                fix nₒᵤₜ = ((Nₒᵤₜ / L) * t);

                auto simHistory = Slab::New<Models::SimHistory>(simulationConfig,
                                                 (Resolution) Nₒᵤₜ,
                                                 (Resolution) nₒᵤₜ,
                                                 xMin,
                                                 L);

                auto ftHistory = Slab::New<Models::SimHistory_DFT>(simulationConfig, nₒᵤₜ);

                outputManager->addOutputChannel(simHistory);
                outputManager->addOutputChannel(ftHistory);

                outputOpenGL->setSimulationHistory(Slab::DummyPointer(simHistory->getData()));
                outputOpenGL->setSpaceFourierHistory(Slab::DummyPointer(ftHistory->getData()),
                                                     ftHistory->getDFTDataHistory());

                if (*OpenGLMonitor_stepsPerIdleCall < 0) {
                    outputOpenGL->setnSteps(simHistory->getnSteps());
                    outputOpenGL->setAutoAdjust_nSteps(true);
                } else {
                    outputOpenGL->setnSteps(*OpenGLMonitor_stepsPerIdleCall);
                    outputOpenGL->setAutoAdjust_nSteps(false);
                }
            }

            guiBackend.addEventListener(Graphics::Window_ptr(outputOpenGL));
            outputManager->addOutputChannel(outputOpenGL);
        } else
            /* O objetivo de relacionar o numero de passos para
             * o Console Monitor com o do file output eh para que
             * ambos possam ficar sincronizados e o integrador
             * possa rodar diversos passos antes de fazer o output. */
            outputManager->addOutputChannel(
                    Slab::New<OutputConsoleMonitor>(simulationConfig.numericConfig, fileOutputStepsInterval > 0
                                                                             ? fileOutputStepsInterval * 25
                                                                             : int(p.getn() / 40)));


        return outputManager;

    }

    void *RtoR::KGBuilder::newFunctionArbitrary() {
        const size_t N = simulationConfig.numericConfig.getN();
        const floatt xLeft = simulationConfig.numericConfig.getxMin();
        const floatt xRight = xLeft + simulationConfig.numericConfig.getL();

        auto laplacianType = periodicBC
                             ? RtoR::DiscreteFunction::Standard1D_PeriodicBorder
                             : RtoR::DiscreteFunction::Standard1D_FixedBorder;

        if (simulationConfig.dev == CPU)
            return new RtoR::DiscreteFunction_CPU(N, xLeft, xRight, laplacianType);

#if USE_CUDA
        else if(simulationConfig.dev==GPU)
            return new RtoR::DiscreteFunctionGPU(N, xLeft, xRight, laplacianType);
#endif

        throw "Error while instantiating Field: device not recognized.";
    }

    void *RtoR::KGBuilder::newFieldState() {
        return new RtoR::EquationState((RtoR::DiscreteFunction *) this->newFunctionArbitrary(),
                                       (RtoR::DiscreteFunction *) this->newFunctionArbitrary());
    }

    void *RtoR::KGBuilder::buildEquationSolver() {
        auto potential = getPotential();
        auto nonHomogenous = getNonHomogenous();
        auto dphi = (RtoR::BoundaryCondition *) getBoundary();

#if USE_CUDA == true
        if(simulationConfig.dev == device::GPU) {
            return new RtoR::SystemGordonGPU(simulationConfig.numericConfig, *dphi, *potential);
        }
#endif

        return new Models::Solver<RtoR::EquationState>(simulationConfig.numericConfig,
                                                                    *dphi, *potential, nonHomogenous);
    }

    auto RtoR::KGBuilder::buildOpenGLOutput() -> void * {
        return new RtoR::Monitor(simulationConfig.numericConfig, *(KGEnergy *) getHamiltonian());
    }

    auto RtoR::KGBuilder::getInitialState() -> void * {
        auto &u_0 = *(RtoR::EquationState *) newFieldState();

        u_0.setPhi(RtoR::NullFunction());
        u_0.setDPhiDt(RtoR::NullFunction());

        return &u_0;
    }

    auto RtoR::KGBuilder::buildStepper() -> Stepper * {
        auto &solver = *(RtoR::KGSolver *) buildEquationSolver();

        return new StepperRK4<typename RtoR::EquationState>(solver);
    }

    void *RtoR::KGBuilder::getHamiltonian() {
        static auto potential = RtoR::Function_ptr(getPotential());
        static auto hamiltonian = new KGEnergy(*this, potential);

        return hamiltonian;
    }

    RtoR::Function_ptr RtoR::KGBuilder::getPotential() const {
        if (*Potential == MASSLESS_WAVE_EQ) {
            return Slab::New<RtoR::NullFunction>();
        } else if (*Potential == KLEIN_GORDON_POTENTIAL) {
            return Slab::New<RtoR::HarmonicPotential>(*massSqr);
        } else if (*Potential == SIGNUM_GORDON_POTENTIAL) {
            return Slab::New<RtoR::AbsFunction>();
        }

        throw "Unknown potential";
    }

    Pointer<Base::FunctionT<Real, Real>> RtoR::KGBuilder::getNonHomogenous() {
        return {};
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
        auto strParams = interface->toString({"massSqr"}, " ");
        Log::Debug() << strParams << Log::Flush;
        auto voidSuggestion = VoidBuilder::suggestFileName();
        return voidSuggestion + " " + strParams;
    }


}