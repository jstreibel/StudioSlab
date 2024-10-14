//
// Created by joao on 10/18/21.
//

#include "KG-RtoRBuilder.h"

#include <utility>

#include "Utils/Files.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "KG-RtoRSolver.h"
#include "KG-RtoRBoundaryCondition.h"

#include "Math/Numerics/Output/Format/BinarySOF.h"
#include "Math/Numerics/Output/Format/ResolutionReductionFilter.h"
#include "Math/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "Math/Function/RtoR/Model/Operators/RtoRLaplacian.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionGPU.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NonlinearKGPotential.h"
#include "Math/Numerics/Method/RungeKutta4.h"

#include "Output/SimHistory.h"
#include "Output/SnapshotOutput.h"
#include "Output/DFTSnapshotOutput.h"

#include "Graphics/RtoRMonitor.h"
#include "Models/KleinGordon/RtoR/Output/CenterTimeDFTOutput.h"

constexpr const Slab::Count MAX_SNAPSHOTS = 200;

#define MASSLESS_WAVE_EQ        0
#define KLEIN_GORDON_POTENTIAL  1
#define SIGNUM_GORDON_POTENTIAL 2
#define REGULAR_SG_POTENTIAL    3

#define DONT_REGISTER_IMMEDIATELY false // line not to be touched

namespace Slab::Models::KGRtoR {

    class Filter1D : public Math::SpaceFilterBase{
        DimensionMetaData dim;
    public:
        explicit Filter1D(DimensionMetaData dim) : dim(std::move(dim)) {}

        auto operator()(const OutputPacket &outputInfo) -> DiscreteSpacePair override {
            auto nakedData = outputInfo.GetNakedStateData<KGRtoR::EquationState>();

            auto N_new = dim.getN(0);

            auto phiSpace  = nakedData->getPhi()   .getSpace().hostCopy(N_new);
            auto dphiSpace = nakedData->getDPhiDt().getSpace().hostCopy(N_new);

            return {phiSpace, dphiSpace};
        }

        auto getOutputDim(const Real L) const -> DimensionMetaData override {
            return dim;
        }
    };

    KGRtoRBuilder::KGRtoRBuilder(const Str &name, Str generalDescription, bool doRegister)
            : Models::KGBuilder("RtoR-" + name, std::move(generalDescription),
                                             DONT_REGISTER_IMMEDIATELY) {
        interface->addParameters({&Potential, &massSqr, &N_num});

        if (doRegister) InterfaceManager::getInstance().registerInterface(interface);
    }

    auto KGRtoRBuilder::buildOutputManager() -> Pointer<OutputManager> {
        UseScientificNotation = false;
        RealToStringDecimalPlaces = 7;

        auto outputFileName = Common::GetPWD() + "/" + this->suggestFileName();

        const auto shouldOutputOpenGL = *VisualMonitor;
        const auto shouldOutputHistory = !*noHistoryToFile;

        if (*VisualMonitor) Core::BackendManager::Startup("GLFW");
        else                Core::BackendManager::Startup("Headless");

        const NumericConfig &p = simulationConfig.numericConfig;

        auto outputManager = New <OutputManager> (simulationConfig.numericConfig);

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
            Utils::TouchFolder(snapshotsFolder);


            auto snapshotFilename = snapshotsFolder + suggestFileName();
            outputManager->addOutputChannel(
                    Slab::New<SnapshotOutput>(simulationConfig.numericConfig, snapshotFilename));
        }
        if (*takeSpaceDFTSnapshot) {
            auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            Utils::TouchFolder(snapshotsFolder);

            auto snapshotFilename = snapshotsFolder + suggestFileName();
            outputManager->addOutputChannel(
                    Slab::New<DFTSnapshotOutput>(simulationConfig.numericConfig, snapshotFilename));
        }
        if(*takeTimeDFTSnapshot) {
            addTimeDFTSnapshots(outputManager);
        }



        /* ****************************************************************************************
           *************************** HISTORY OUTPUT *********************************************
           **************************************************************************************** */
        int fileOutputStepsInterval = -1;
        Socket_ptr out;
        if (shouldOutputHistory) {
            OutputFormatterBase *outputFilter = new BinarySOF;

            auto dimData = DimensionMetaData({(unsigned) *outputResolution}, {L / *outputResolution});
            // auto *spaceFilter = new ResolutionReductionFilter(dimData);
            auto *spaceFilter = new Filter1D(dimData);

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

            auto outputOpenGL = Pointer<Monitor>((Monitor*)buildOpenGLOutput());

            if (t > 0) {
                fix nₒᵤₜ = ((Nₒᵤₜ / L) * t);

                auto simHistory = Slab::New<SimHistory>(simulationConfig,
                                                 (Resolution) Nₒᵤₜ,
                                                 (Resolution) nₒᵤₜ,
                                                 xMin,
                                                 L);

                auto ftHistory = Slab::New<SimHistory_DFT>(simulationConfig, nₒᵤₜ);

                outputManager->addOutputChannel(simHistory);
                outputManager->addOutputChannel(ftHistory);

                outputOpenGL->setSimulationHistory(Slab::Naked(simHistory->getData()));
                outputOpenGL->setSpaceFourierHistory(Slab::Naked(ftHistory->getData()),
                                                     ftHistory->getDFTDataHistory());
            }

            guiBackend.addEventListener(Graphics::Window_ptr(outputOpenGL));
            outputManager->addOutputChannel(outputOpenGL);
        } else
            /* O objetivo de relacionar o numero de passos para
             * o Console Monitor com o do file output eh para que
             * ambos possam ficar sincronizados e o integrador
             * possa rodar diversos passos antes de fazer o output. */

            outputManager->addOutputChannel(
                    Slab::New<OutputConsoleMonitor>(simulationConfig.numericConfig));


        return outputManager;

    }

    void KGRtoRBuilder::addTimeDFTSnapshots(Pointer<OutputManager> outputManager) {
        fix t = simulationConfig.numericConfig.gett();

        fix Δt    = *timeDFTSnapshot_tDelta  <= 0 ? t : *timeDFTSnapshot_tDelta;
        fix t_len = *timeDFTSnapshot_tLength <= 0 ? t : *timeDFTSnapshot_tLength;

        Count snapshot_count = std::ceil(t / Δt);

        if(snapshot_count>MAX_SNAPSHOTS)
            throw Exception(
                Str("Error generating time-domain dft snapshots. The value of '--") +
                timeDFTSnapshot_tDelta.getCLName(true) + "' is " + timeDFTSnapshot_tDelta.valueToString() +
                ", yielding a snapshot count of " + ToStr(snapshot_count) + ", which is above the " +
                ToStr(MAX_SNAPSHOTS) + " limit.");

        RealVector x_locations;
        {
            fix L = simulationConfig.numericConfig.getL();
            fix xMin = simulationConfig.numericConfig.getxMin();

            if(auto k_param = InterfaceManager::getInstance().getParameter("k")){
                auto k_n = k_param->getValueAs<Real>();
                x_locations = {L/(4.*k_n)};
            } else
                x_locations={xMin+L/2};
        }

        if(snapshot_count==1) {
            auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            auto snapshotSocket = _newTimeDFTSnapshotOutput(snapshotsFolder, t-t_len, t, x_locations);
            outputManager->addOutputChannel(snapshotSocket);

            return;
        }

        int i=0;
        do {
            fix t_end   = t - i*Δt;
            fix t_start = Common::max(t_end-t_len, 0.0);
            const UInt decimal_places = t_end > 10 ? 0 : t_end>1 ? 1 : 0;

            auto snapshotsFolder = Common::GetPWD() +
                    "/snapshots_t" + ToStr(t_start, decimal_places) +  "-" + ToStr(t_end, decimal_places) + "/";
            auto snapshotSocket = _newTimeDFTSnapshotOutput(snapshotsFolder, t_start, t_end, x_locations);
            outputManager->addOutputChannel(snapshotSocket);
        } while (snapshot_count >++ i);
    }

    Pointer<Socket>
    KGRtoRBuilder::_newTimeDFTSnapshotOutput(Str folder, Real t_start, Real t_end, RealVector x_locations) {

        Utils::TouchFolder(folder);

        auto snapshotFilename = folder + suggestFileName();
        TimeDFTOutputConfig dftConfig = {snapshotFilename, x_locations, t_start, t_end};

        return Slab::New<CenterTimeDFTOutput>(simulationConfig.numericConfig, dftConfig);
    }

    RtoR::NumericFunction_ptr KGRtoRBuilder::newFunctionArbitrary() {
        const size_t N = simulationConfig.numericConfig.getN();
        const floatt xLeft = simulationConfig.numericConfig.getxMin();
        const floatt xRight = xLeft + simulationConfig.numericConfig.getL();

        auto laplacianType = periodicBC
                             ? RtoR::NumericFunction::Standard1D_PeriodicBorder
                             : RtoR::NumericFunction::Standard1D_FixedBorder;

        if (simulationConfig.dev == CPU)
            return New<RtoR::NumericFunction_CPU>(N, xLeft, xRight, laplacianType);

#if USE_CUDA == true
        else if(simulationConfig.dev==GPU)
            return New<RtoR::NumericFunctionGPU>(N, xLeft, xRight, laplacianType);
#endif

        throw Exception("Error while instantiating Field: device not recognized.");
    }

    EquationState_ptr KGRtoRBuilder::newFieldState() {
        return New<EquationState>(this->newFunctionArbitrary(),
                                       this->newFunctionArbitrary());
    }

    Base::Solver_ptr KGRtoRBuilder::buildEquationSolver() {
        auto potential = getPotential();
        auto nonHomogenous = getNonHomogenous();
        auto dphi = getBoundary();

#if USE_CUDA == true
        /*
        if(simulationConfig.dev == device::GPU) {
            return New<RtoR::SystemGordonGPU>(simulationConfig.numericConfig, *dphi, *potential);
        }
         */
#endif
        auto &params = simulationConfig.numericConfig;
        auto Laplacian = New <Math::RtoR::RtoRLaplacian> ();

        auto solver = New<KGRtoRSolver>(params, dphi, Laplacian, potential, nonHomogenous);

        return solver;
    }

    auto KGRtoRBuilder::buildOpenGLOutput() -> void * {
        return new Monitor(simulationConfig.numericConfig, *(KGEnergy *) getHamiltonian());
    }

    auto KGRtoRBuilder::getInitialState() -> KGRtoR::EquationState_ptr {
        auto u_0 = newFieldState();

        u_0->setPhi(RtoR::NullFunction());
        u_0->setDPhiDt(RtoR::NullFunction());

        return u_0;
    }

    void *KGRtoRBuilder::getHamiltonian() {
        static auto potential = RtoR::Function_ptr(getPotential());
        static auto hamiltonian = new KGEnergy(potential);

        return hamiltonian;
    }

    RtoR::Function_ptr KGRtoRBuilder::getPotential() const {
        if (*Potential == MASSLESS_WAVE_EQ) {
            return Slab::New<RtoR::NullFunction>();
        } else if (*Potential == KLEIN_GORDON_POTENTIAL) {
            return Slab::New<RtoR::HarmonicPotential>(*massSqr);
        } else if (*Potential == SIGNUM_GORDON_POTENTIAL) {
            return Slab::New<RtoR::AbsFunction>();
        } else if (*Potential == REGULAR_SG_POTENTIAL) {
            fix m2 = *massSqr;
            fix A = 4/M_PI/m2;
            return Slab::New<Slab::Math::RtoR::NonlinearKGPotential>(A, *N_num, 1.0);
        }

        throw Exception("Unknown potential");
    }

    Pointer<Base::FunctionT<Real, Real>> KGRtoRBuilder::getNonHomogenous() {
        return {};
    }

    void KGRtoRBuilder::setLaplacianPeriodicBC() {
        Log::Info() << "KGBuilder Laplacian set to PERIODIC borders." << Log::Flush;
        periodicBC = true;
    }

    void KGRtoRBuilder::setLaplacianFixedBC() {
        Log::Info() << "KGBuilder Laplacian set to FIXED borders." << Log::Flush;
        periodicBC = false;
    }

    bool KGRtoRBuilder::usesPeriodicBC() const { return periodicBC; }

    Str KGRtoRBuilder::suggestFileName() const {
        auto strParams = interface->toString({"massSqr"}, " ");
        Log::Debug() << strParams << Log::Flush;
        auto voidSuggestion = NumericalRecipe::suggestFileName();
        return voidSuggestion + " " + strParams;
    }




}