//
// Created by joao on 10/18/21.
//

#include "KG-RtoRBuilder.h"

#include <utility>

#include "Utils/Files.h"

#include "Core/Backend/BackendManager.h"
#include "Core/SlabCore.h"
#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"

#include "KG-RtoRSolver.h"
#include "KG-RtoRBoundaryCondition.h"

#include "Math/Data/DataAllocator.h"
#include "Math/Numerics/ODE/Output/Format/BinarySOF.h"
#include "Math/Numerics/ODE/Output/Format/ResolutionReductionFilter.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputHistoryToFile.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"
#include "Math/Function/RtoR/Model/Operators/RtoRLaplacian.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NonlinearKGPotential.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionGPU.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"

#include "Models/KleinGordon/RtoR/LinearStepping/Output/SimHistory.h"
#include "Models/KleinGordon/RtoR/LinearStepping/Output/SnapshotOutput.h"
#include "Models/KleinGordon/RtoR/LinearStepping/Output/DFTSnapshotOutput.h"
#include "Models/KleinGordon/RtoR/Graphics/RtoRMonitor.h"
#include "Models/KleinGordon/RtoR/LinearStepping/Output/CenterTimeDFTOutput.h"

#include "Graphics/Window/SlabWindowManager.h"

constexpr const Slab::CountType MAX_SNAPSHOTS = 200;

#define MASSLESS_WAVE_EQ        0
#define KLEIN_GORDON_POTENTIAL  1
#define SIGNUM_GORDON_POTENTIAL 2
#define REGULAR_SG_POTENTIAL    3

#define DONT_REGISTER_IMMEDIATELY false // line not to be touched

namespace Slab::Models::KGRtoR {

    class Filter1D final : public Math::SpaceFilterBase{
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

    KGRtoRBuilder::KGRtoRBuilder(const Str &name, const Str& generalDescription, bool doRegister)
            : Models::KGRecipe(New<KGNumericConfig>(false), "RtoR-" + name, generalDescription,
                               DONT_REGISTER_IMMEDIATELY) {
        Interface->AddParameters({&Potential, &massSqr, &N_num, &BoundaryConditions});

        if (doRegister) RegisterCLInterface(Interface);
    }

    auto KGRtoRBuilder::buildOutputSockets() -> Vector<TPointer<Socket>> {
        Vector<TPointer<Socket>> Sockets;

        UseScientificNotation = false;
        RealToStringDecimalPlaces = 7;

        auto outputFileName = Common::GetPWD() + "/" + this->SuggestFileName();

        const auto shouldOutputOpenGL = *VisualMonitor;
        const auto shouldOutputHistory = !*noHistoryToFile;

        // if (*VisualMonitor) Core::BackendManager::Startup("GLFW");
        // else                Core::BackendManager::Startup("Headless");

        const KGNumericConfig &p = dynamic_cast<KGNumericConfig&>(*numeric_config);

        fix t = p.gett();
        fix max_steps = p.getn();
        fix N = static_cast<DevFloat>(p.getN());
        fix L = p.GetL();
        fix xMin = p.getxMin();
        fix Nₒᵤₜ = *outputResolution > N ? N : *outputResolution;
        fix r = p.getr();

        /* ****************************************************************************************
           *************************** SNAPSHOT OUTPUT ********************************************
           **************************************************************************************** */
        if (*takeSnapshot) {
            const auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            Utils::TouchFolder(snapshotsFolder);

            auto snapshotFilename = snapshotsFolder + SuggestFileName();
            Sockets.emplace_back(Slab::New<SnapshotOutput>(snapshotFilename));
        }
        if (*takeSpaceDFTSnapshot) {
            const auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            Utils::TouchFolder(snapshotsFolder);

            auto snapshotFilename = snapshotsFolder + SuggestFileName();
            Sockets.emplace_back(Slab::New<DFTSnapshotOutput>(N, L, snapshotFilename));
        }
        if(*takeTimeDFTSnapshot) {
            auto time_dftsnapshots = getTimeDFTSnapshots();

            for(auto &socket : time_dftsnapshots)
                Sockets.emplace_back(socket);
        }



        /* ****************************************************************************************
           *************************** HISTORY OUTPUT *********************************************
           **************************************************************************************** */
        if (shouldOutputHistory) {
            OutputFormatterBase *outputFilter = new BinarySOF;

            const auto dimData = DimensionMetaData({(unsigned) *outputResolution}, {L / *outputResolution});
            // auto *spaceFilter = new ResolutionReductionFilter(dimData);
            auto *spaceFilter = new Filter1D(dimData);

            fix stepsInterval = static_cast<UInt>(N / (Nₒᵤₜ * r));

            Socket_ptr out = Slab::New<OutputHistoryToFile>(stepsInterval, spaceFilter, outputFileName, outputFilter);
            Sockets.emplace_back(out);
        }



        /* ****************************************************************************************
           *************************** VISUAL MONITOR *********************************************
           **************************************************************************************** */
        if (shouldOutputOpenGL) {
            auto GuiBackend = Slab::Graphics::GetGraphicsBackend();

            auto outputOpenGL = TPointer<Monitor>(static_cast<Monitor *>(buildOpenGLOutput()));

            if (t > 0) {
                fix nₒᵤₜ = ((Nₒᵤₜ / L) * t);

                auto simHistory = Slab::New<SimHistory>(max_steps, t,
                                                 static_cast<Resolution>(Nₒᵤₜ),
                                                 (Resolution) nₒᵤₜ,
                                                 xMin,
                                                 L);

                auto ftHistory = Slab::New<SimHistory_DFT>(max_steps, t, N, L, nₒᵤₜ);

                Sockets.emplace_back(simHistory);
                Sockets.emplace_back(ftHistory);

                outputOpenGL->setSimulationHistory(Slab::Naked(simHistory->getData()));
                outputOpenGL->SetSpaceFourierHistory(Slab::Naked(ftHistory->getData()),
                                                     ftHistory->GetDFTDataHistory());
            }

            const auto WindowManager = New<SlabWindowManager>();
            GuiBackend->GetMainSystemWindow()->AddAndOwnEventListener(WindowManager);

            auto Window = TPointer<FSlabWindow>(outputOpenGL);
            WindowManager->AddSlabWindow(Window, false);

            Sockets.emplace_back(outputOpenGL);
        } else {
            Sockets.emplace_back(Slab::New<OutputConsoleMonitor>(max_steps, max_steps/2));
        }

        return Sockets;

    }

    Vector<TPointer<Socket>> KGRtoRBuilder::getTimeDFTSnapshots() {
        auto &c = *kg_numeric_config;

        fix t = c.gett();

        fix Δt    = *timeDFTSnapshot_tDelta  <= 0 ? t : *timeDFTSnapshot_tDelta;
        fix t_len = *timeDFTSnapshot_tLength <= 0 ? t : *timeDFTSnapshot_tLength;

        const CountType snapshot_count = std::ceil(t / Δt);

        if(snapshot_count>MAX_SNAPSHOTS)
            throw Exception(
                    Str("Error generating time-domain dft snapshots. The value of '--") +
                            timeDFTSnapshot_tDelta.getCommandLineArgumentName(true) + "' is " + timeDFTSnapshot_tDelta.ValueToString() +
                    ", yielding a snapshot count of " + ToStr(snapshot_count) + ", which is above the " +
                ToStr(MAX_SNAPSHOTS) + " limit.");

        FRealVector x_locations;
        {
            fix L = c.GetL();
            fix xMin = c.getxMin();

            if(fix k_param = FCommandLineInterfaceManager::getInstance().getParameter("k")){
                fix k_n = k_param->getValueAs<DevFloat>();
                x_locations = {L/(4.*k_n)};
            } else
                x_locations={xMin+L/2};
        }

        if(snapshot_count==1) {
            fix snapshotsFolder = Common::GetPWD() + "/snapshots/";
            auto snapshotSocket = _newTimeDFTSnapshotOutput(snapshotsFolder, t-t_len, t, x_locations);

            return {snapshotSocket};
        }

        int i=0;
        Vector<TPointer<Socket>> sockets;
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

    TPointer<Socket>
    KGRtoRBuilder::_newTimeDFTSnapshotOutput(const Str& folder, const DevFloat t_start, const DevFloat t_end, const FRealVector &x_locations) const {

        Utils::TouchFolder(folder);

        const auto snapshotFilename = folder + SuggestFileName();
        TimeDFTOutputConfig dftConfig = {snapshotFilename, x_locations, t_start, t_end};

        fix &conf = *kg_numeric_config;
        return Slab::New<CenterTimeDFTOutput>(conf.gett(), conf.getn(), dftConfig);
    }

    RtoR::NumericFunction_ptr KGRtoRBuilder::newFunctionArbitrary() const {
        fix &conf = *kg_numeric_config;

        const size_t N = conf.getN();
        const floatt xLeft = conf.getxMin();
        const floatt xRight = xLeft + conf.GetL();

        auto laplacianType = force_periodicBC || *BoundaryConditions == 1
                             ? RtoR::NumericFunction::Standard1D_PeriodicBorder
                             : RtoR::NumericFunction::Standard1D_FixedBorder;

        if (device_config == CPU)
            return Math::DataAlloc<RtoR::NumericFunction_CPU>("IntegrationData", N, xLeft, xRight, laplacianType);

#if USE_CUDA == true
        if(device_config == Device::GPU)
            return New<RtoR::NumericFunctionGPU>(N, xLeft, xRight, laplacianType);
#endif

        throw Exception("Error while instantiating Field: device not recognized.");
    }

    EquationState_ptr KGRtoRBuilder::NewFieldState() const {
        return New<EquationState>(this->newFunctionArbitrary(),
                                       this->newFunctionArbitrary());
    }

    TPointer<Base::LinearStepSolver> KGRtoRBuilder::buildSolver() {
        auto potential = getPotential();
        auto nonHomogenous = GetNonHomogenousTerm();
        auto dphi = GetBoundary();

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

    auto KGRtoRBuilder::BuildOpenGLOutput() -> void * {
        return new Monitor(kg_numeric_config, *static_cast<KGEnergy *>(getHamiltonian()));
    }

    auto KGRtoRBuilder::getInitialState() const -> KGRtoR::EquationState_ptr {
        auto u_0 = NewFieldState();

        u_0->SetPhi(RtoR::NullFunction());
        u_0->SetDPhiDt(RtoR::NullFunction());

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
        }
        if (*Potential == KLEIN_GORDON_POTENTIAL) {
            return Slab::New<RtoR::HarmonicPotential>(*massSqr);
        }
        if (*Potential == SIGNUM_GORDON_POTENTIAL) {
            return Slab::New<RtoR::AbsFunction>();
        }
        if (*Potential == REGULAR_SG_POTENTIAL) {
            fix m2 = *massSqr;
            fix A = 4/M_PI/m2;
            return Slab::New<Slab::Math::RtoR::NonlinearKGPotential>(A, *N_num, 1.0);
        }

        throw Exception("Unknown potential");
    }

    TPointer<Base::FunctionT<DevFloat, DevFloat>> KGRtoRBuilder::GetNonHomogenousTerm() {
        return {};
    }

    void KGRtoRBuilder::SetLaplacianPeriodicBC() {
        Log::Attention() << "KGBuilder Laplacian forced to PERIODIC borders." << Log::Flush;
        force_periodicBC = true;
    }

    void KGRtoRBuilder::SetLaplacianFixedBC() {
        // Log::Info() << "KGBuilder Laplacian set to FIXED borders." << Log::Flush;
        force_periodicBC = false;
    }

    bool KGRtoRBuilder::usesPeriodicBC() const { return force_periodicBC; }

    Str KGRtoRBuilder::SuggestFileName() const {
        const auto strParams = Interface->ToString({"massSqr"}, " ");
        Log::Debug() << strParams << Log::Flush;
        const auto voidSuggestion = NumericalRecipe::SuggestFileName();
        return voidSuggestion + " " + strParams;
    }




}