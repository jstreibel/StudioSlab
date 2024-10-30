//
// Created by joao on 10/18/21.
//

#include "KG-RtoRBuilder.h"

#include <utility>

#include "Utils/Files.h"

#include "Core/Backend/BackendManager.h"
#include "Core/SlabCore.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include "KG-RtoRSolver.h"
#include "KG-RtoRBoundaryCondition.h"

#include "Math/Numerics/ODE/Output/Format/BinarySOF.h"
#include "Math/Numerics/ODE/Output/Format/ResolutionReductionFilter.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputHistoryToFile.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"
#include "Math/Function/RtoR/Model/Operators/RtoRLaplacian.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NonlinearKGPotential.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"

#include "Models/KleinGordon/RtoR/LinearStepping/Output/SimHistory.h"
#include "Models/KleinGordon/RtoR/LinearStepping/Output/SnapshotOutput.h"
#include "Models/KleinGordon/RtoR/LinearStepping/Output/DFTSnapshotOutput.h"

#include "Models/KleinGordon/RtoR/Graphics/RtoRMonitor.h"
#include "Models/KleinGordon/RtoR/LinearStepping/Output/CenterTimeDFTOutput.h"
#include "Math/Data/DataAllocator.h"
#include "Graphics/Window/SlabWindowManager.h"

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

        auto getOutputDim() const -> DimensionMetaData override {
            return dim;
        }
    };

    KGRtoRBuilder::KGRtoRBuilder(const Str &name, Str generalDescription, bool doRegister)
            : Models::KGRecipe(New<KGNumericConfig>(false), "RtoR-" + name, std::move(generalDescription),
                               DONT_REGISTER_IMMEDIATELY) {
        interface->addParameters({&Potential, &massSqr, &N_num});

        if (doRegister) RegisterCLInterface(interface);
    }

    auto KGRtoRBuilder::buildOutputSockets() -> Vector<Pointer<Socket>> {
        Vector<Pointer<Socket>> sockets;

        UseScientificNotation = false;
        RealToStringDecimalPlaces = 7;

        auto outputFileName = Common::GetPWD() + "/" + this->suggestFileName();

        const auto shouldOutputOpenGL = *VisualMonitor;
        const auto shouldOutputHistory = !*noHistoryToFile;

        if (*VisualMonitor) Core::BackendManager::Startup("GLFW");
        else                Core::BackendManager::Startup("Headless");

        const KGNumericConfig &p = dynamic_cast<KGNumericConfig&>(*numeric_config);

        fix t = p.gett();
        fix max_steps = p.getn();
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
            sockets.emplace_back(Slab::New<SnapshotOutput>(snapshotFilename));
        }
        if (*takeSpaceDFTSnapshot) {
            auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            Utils::TouchFolder(snapshotsFolder);

            auto snapshotFilename = snapshotsFolder + suggestFileName();
            sockets.emplace_back(Slab::New<DFTSnapshotOutput>(N, L, snapshotFilename));
        }
        if(*takeTimeDFTSnapshot) {
            auto time_dftsnapshots = getTimeDFTSnapshots();

            for(auto &socket : time_dftsnapshots)
                sockets.emplace_back(socket);
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

            out = Slab::New<OutputHistoryToFile>(stepsInterval, spaceFilter, outputFileName, outputFilter);
            fileOutputStepsInterval = out->getnSteps();
            sockets.emplace_back(out);
        }



        /* ****************************************************************************************
           *************************** VISUAL MONITOR *********************************************
           **************************************************************************************** */
        if (shouldOutputOpenGL) {
            auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

            auto outputOpenGL = Pointer<Monitor>((Monitor*)buildOpenGLOutput());

            if (t > 0) {
                fix nₒᵤₜ = ((Nₒᵤₜ / L) * t);

                auto simHistory = Slab::New<SimHistory>(max_steps, t,
                                                 (Resolution) Nₒᵤₜ,
                                                 (Resolution) nₒᵤₜ,
                                                 xMin,
                                                 L);

                auto ftHistory = Slab::New<SimHistory_DFT>(max_steps, t, N, L, nₒᵤₜ);

                sockets.emplace_back(simHistory);
                sockets.emplace_back(ftHistory);

                outputOpenGL->setSimulationHistory(Slab::Naked(simHistory->getData()));
                outputOpenGL->setSpaceFourierHistory(Slab::Naked(ftHistory->getData()),
                                                     ftHistory->getDFTDataHistory());
            }

            auto wm = New<Graphics::SlabWindowManager>();
            wm->addSlabWindow(Pointer<Graphics::SlabWindow>(outputOpenGL));
            guiBackend.addAndOwnEventListener(wm);
            sockets.emplace_back(outputOpenGL);
        } else {
            /* O objetivo de relacionar o numero de passos para
             * o Console Monitor com o do file output eh para que
             * ambos possam ficar sincronizados e o integrador
             * possa rodar diversos passos antes de fazer o output. */

            sockets.emplace_back(Slab::New<OutputConsoleMonitor>(max_steps));
        }

        return sockets;

    }

    Vector<Pointer<Socket>> KGRtoRBuilder::getTimeDFTSnapshots() {
        auto &c = *kg_numeric_config;

        fix t = c.gett();

        fix Δt    = *timeDFTSnapshot_tDelta  <= 0 ? t : *timeDFTSnapshot_tDelta;
        fix t_len = *timeDFTSnapshot_tLength <= 0 ? t : *timeDFTSnapshot_tLength;

        Count snapshot_count = std::ceil(t / Δt);

        if(snapshot_count>MAX_SNAPSHOTS)
            throw Exception(
                    Str("Error generating time-domain dft snapshots. The value of '--") +
                            timeDFTSnapshot_tDelta.getCommandLineArgumentName(true) + "' is " + timeDFTSnapshot_tDelta.valueToString() +
                    ", yielding a snapshot count of " + ToStr(snapshot_count) + ", which is above the " +
                ToStr(MAX_SNAPSHOTS) + " limit.");

        RealVector x_locations;
        {
            fix L = c.getL();
            fix xMin = c.getxMin();

            if(auto k_param = CLInterfaceManager::getInstance().getParameter("k")){
                auto k_n = k_param->getValueAs<Real>();
                x_locations = {L/(4.*k_n)};
            } else
                x_locations={xMin+L/2};
        }

        if(snapshot_count==1) {
            auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            auto snapshotSocket = _newTimeDFTSnapshotOutput(snapshotsFolder, t-t_len, t, x_locations);

            return {snapshotSocket};
        }

        int i=0;
        Vector<Pointer<Socket>> sockets;
        do {
            fix t_end   = t - i*Δt;
            fix t_start = Common::max(t_end-t_len, 0.0);
            const UInt decimal_places = t_end > 10 ? 0 : t_end>1 ? 1 : 0;

            auto snapshotsFolder = Common::GetPWD() +
                    "/snapshots_t" + ToStr(t_start, decimal_places) +  "-" + ToStr(t_end, decimal_places) + "/";
            auto snapshotSocket = _newTimeDFTSnapshotOutput(snapshotsFolder, t_start, t_end, x_locations);
            sockets.emplace_back(snapshotSocket);
        } while (snapshot_count >++ i);

        return sockets;
    }

    Pointer<Socket>
    KGRtoRBuilder::_newTimeDFTSnapshotOutput(Str folder, Real t_start, Real t_end, RealVector x_locations) {

        Utils::TouchFolder(folder);

        auto snapshotFilename = folder + suggestFileName();
        TimeDFTOutputConfig dftConfig = {snapshotFilename, x_locations, t_start, t_end};

        fix &conf = *kg_numeric_config;
        return Slab::New<CenterTimeDFTOutput>(conf.gett(), conf.getn(), dftConfig);
    }

    RtoR::NumericFunction_ptr KGRtoRBuilder::newFunctionArbitrary() {
        fix &conf = *kg_numeric_config;

        const size_t N = conf.getN();
        const floatt xLeft = conf.getxMin();
        const floatt xRight = xLeft + conf.getL();

        auto laplacianType = periodicBC
                             ? RtoR::NumericFunction::Standard1D_PeriodicBorder
                             : RtoR::NumericFunction::Standard1D_FixedBorder;

        if (device_config == CPU)
            return Math::DataAlloc<RtoR::NumericFunction_CPU>("IntegrationData", N, xLeft, xRight, laplacianType);

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

    Pointer<Base::LinearStepSolver> KGRtoRBuilder::buildSolver() {
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
        auto Laplacian = New <Math::RtoR::RtoRLaplacian> ();

        auto solver = New<KGRtoRSolver>(dphi, Laplacian, potential, nonHomogenous);

        return solver;
    }

    auto KGRtoRBuilder::buildOpenGLOutput() -> void * {
        return new Monitor(kg_numeric_config, *(KGEnergy *) getHamiltonian());
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