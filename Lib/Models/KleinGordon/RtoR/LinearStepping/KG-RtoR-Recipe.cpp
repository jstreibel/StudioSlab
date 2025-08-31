//
// Created by joao on 10/18/21.
//

#include "KG-RtoR-Recipe.h"

#include <utility>

#include "Utils/Files.h"

#include "Core/Backend/BackendManager.h"
#include "Core/SlabCore.h"
#include "../../../../Core/Controller/InterfaceManager.h"

#include "KG-RtoRSolver.h"
#include "KG-RtoRBoundaryCondition.h"
#include "Graphics/Window/SingleWindowManager.h"

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

    FKGRtoRPotentialOptions::FKGRtoRPotentialOptions(bool bDoRegister)
    : FInterfaceOwner("Options for the KG Potential", 10000, false)
    {
        Interface->AddParameters({&Potential, &MassSquared, &N_num});

        if (bDoRegister) RegisterCLInterface(Interface);
    }

    class Filter1D final : public Math::SpaceFilterBase{
        DimensionMetaData dim;
    public:
        explicit Filter1D(DimensionMetaData dim) : dim(std::move(dim)) {}

        auto operator()(const FOutputPacket &outputInfo) -> DiscreteSpacePair override {
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

    FKGRtoR_Recipe::FKGRtoR_Recipe(const Str &Name, const Str& generalDescription, bool doRegister)
    : KGRecipe(New<FKGNumericConfig>(false), Name, generalDescription, DONT_REGISTER_IMMEDIATELY)
    , PotentialOptions(false) {
        Interface->AddParameters({&BoundaryConditions});

        Interface->AddSubInterface(PotentialOptions.GetInterface());

        if (doRegister) RegisterCLInterface(Interface);
    }

    auto FKGRtoR_Recipe::BuildOutputSockets() -> Vector<TPointer<FOutputChannel>> {
        Vector<TPointer<FOutputChannel>> Sockets;

        UseScientificNotation = false;
        RealToStringDecimalPlaces = 7;

        auto outputFileName = Common::GetPWD() + "/" + this->SuggestFileName();

        const auto shouldOutputOpenGL = *OutputOptions.VisualMonitor;
        const auto shouldOutputHistory = !*OutputOptions.NoHistoryToFile;

        // if (*VisualMonitor) Core::BackendManager::Startup("GLFW");
        // else                Core::BackendManager::Startup("Headless");

        const FKGNumericConfig &p = dynamic_cast<FKGNumericConfig&>(*NumericConfig);

        fix t = p.Get_t();
        fix max_steps = p.Get_n();
        fix N = static_cast<DevFloat>(p.getN());
        fix L = p.GetL();
        fix xMin = p.Get_xMin();
        fix Nₒᵤₜ = *OutputOptions.OutputResolution > N ? N : *OutputOptions.OutputResolution;
        fix r = p.Get_r();

        /* ****************************************************************************************
           *************************** SNAPSHOT OUTPUT ********************************************
           **************************************************************************************** */
        if (*OutputOptions.TakeSnapshot) {
            const auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            Utils::TouchFolder(snapshotsFolder);

            auto snapshotFilename = snapshotsFolder + SuggestFileName();
            Sockets.emplace_back(Slab::New<SnapshotOutput>(snapshotFilename));
        }
        if (*OutputOptions.TakeSpaceDFTSnapshot) {
            const auto snapshotsFolder = Common::GetPWD() + "/snapshots/";
            Utils::TouchFolder(snapshotsFolder);

            auto snapshotFilename = snapshotsFolder + SuggestFileName();
            Sockets.emplace_back(Slab::New<DFTSnapshotOutput>(N, L, snapshotFilename));
        }
        if(*OutputOptions.TakeTimeDFTSnapshot) {
            auto time_dftsnapshots = getTimeDFTSnapshots();

            for(auto &socket : time_dftsnapshots)
                Sockets.emplace_back(socket);
        }



        /* ****************************************************************************************
           *************************** HISTORY OUTPUT *********************************************
           **************************************************************************************** */
        if (shouldOutputHistory) {
            OutputFormatterBase *outputFilter = new BinarySOF;

            const auto dimData = DimensionMetaData({(unsigned) *OutputOptions.OutputResolution}, {L / *OutputOptions.OutputResolution});
            // auto *spaceFilter = new ResolutionReductionFilter(dimData);
            auto *spaceFilter = new Filter1D(dimData);

            fix stepsInterval = static_cast<UInt>(N / (Nₒᵤₜ * r));

            FOutputChannel_ptr out = Slab::New<OutputHistoryToFile>(stepsInterval, spaceFilter, outputFileName, outputFilter);
            Sockets.emplace_back(out);
        }



        /* ****************************************************************************************
           *************************** VISUAL MONITOR *********************************************
           **************************************************************************************** */
        if (shouldOutputOpenGL) {
            auto GuiBackend = Slab::Graphics::GetGraphicsBackend();

            auto outputOpenGL = TPointer<Monitor>(static_cast<Monitor *>(BuildOpenGLOutput()));

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

                outputOpenGL->SetSimulationHistory(simHistory->GetData());
                outputOpenGL->SetSpaceFourierHistory(ftHistory->GetData(),
                                                     ftHistory->GetDFTDataHistory());
            }

            // const auto WindowManager = New<FSlabWindowManager>();
            const auto WindowManager = New<FSingleWindowManager>();
            GuiBackend->GetMainSystemWindow()->AddAndOwnEventListener(WindowManager);

            auto Window = TPointer<FSlabWindow>(outputOpenGL);
            WindowManager->AddSlabWindow(Window, false);

            Sockets.emplace_back(outputOpenGL);
        } else {
            Sockets.emplace_back(Slab::New<OutputConsoleMonitor>(max_steps, max_steps/2));
        }

        return Sockets;

    }

    Vector<TPointer<FOutputChannel>> FKGRtoR_Recipe::getTimeDFTSnapshots() {
        auto &c = *KGNumericConfig;

        fix t = c.Get_t();

        fix Δt    = *OutputOptions.TimeDFTSnapshot_tDelta  <= 0 ? t : *OutputOptions.TimeDFTSnapshot_tDelta;
        fix t_len = *OutputOptions.TimeDFTSnapshot_tLength <= 0 ? t : *OutputOptions.TimeDFTSnapshot_tLength;

        const CountType snapshot_count = std::ceil(t / Δt);

        if(snapshot_count>MAX_SNAPSHOTS)
            throw Exception(
                Str("Error generating time-domain dft snapshots. The value of '--")
                + OutputOptions.TimeDFTSnapshot_tDelta.GetCommandLineArgumentName(true)
                + "' is " + OutputOptions.TimeDFTSnapshot_tDelta.ValueToString()
                + ", yielding a snapshot count of " + ToStr(snapshot_count)
                + ", which is above the " + ToStr(MAX_SNAPSHOTS) + " limit.");

        FRealVector x_locations;
        {
            fix L = c.GetL();
            fix xMin = c.Get_xMin();

            if(fix k_param = FInterfaceManager::GetInstance().GetParameter("k")){
                fix k_n = k_param->GetValueAs<DevFloat>();
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
        Vector<TPointer<FOutputChannel>> sockets;
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

    TPointer<FOutputChannel>
    FKGRtoR_Recipe::_newTimeDFTSnapshotOutput(const Str& folder, const DevFloat t_start, const DevFloat t_end, const FRealVector &x_locations) const {

        Utils::TouchFolder(folder);

        const auto snapshotFilename = folder + SuggestFileName();
        TimeDFTOutputConfig dftConfig = {snapshotFilename, x_locations, t_start, t_end};

        fix &conf = *KGNumericConfig;
        return Slab::New<CenterTimeDFTOutput>(conf.Get_t(), conf.Get_n(), dftConfig);
    }

    RtoR::NumericFunction_ptr FKGRtoR_Recipe::newFunctionArbitrary() const {
        fix &conf = *KGNumericConfig;

        const size_t N = conf.getN();
        const floatt xLeft = conf.Get_xMin();
        const floatt xRight = xLeft + conf.GetL();

        auto laplacianType = force_periodicBC || *BoundaryConditions == 1
                             ? RtoR::NumericFunction::Standard1D_PeriodicBorder
                             : RtoR::NumericFunction::Standard1D_FixedBorder;

        if (DeviceConfig == CPU)
            return Math::DataAlloc<RtoR::NumericFunction_CPU>("IntegrationData", N, xLeft, xRight, laplacianType);

#if USE_CUDA == true
        if(DeviceConfig == Device::GPU)
            return New<RtoR::NumericFunctionGPU>(N, xLeft, xRight, laplacianType);
#endif

        throw Exception("Error while instantiating Field: device not recognized.");
    }

    EquationState_ptr FKGRtoR_Recipe::NewFieldState() const {
        return New<EquationState>(this->newFunctionArbitrary(),
                                       this->newFunctionArbitrary());
    }

    TPointer<Base::LinearStepSolver> FKGRtoR_Recipe::buildSolver() {
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

    auto FKGRtoR_Recipe::BuildOpenGLOutput() -> void * {
        return new Monitor(KGNumericConfig, *static_cast<FKGEnergy *>(getHamiltonian()));
    }

    auto FKGRtoR_Recipe::getInitialState() const -> KGRtoR::EquationState_ptr {
        auto u_0 = NewFieldState();

        u_0->SetPhi(RtoR::NullFunction());
        u_0->SetDPhiDt(RtoR::NullFunction());

        return u_0;
    }

    void *FKGRtoR_Recipe::getHamiltonian() {
        static auto potential = RtoR::Function_ptr(getPotential());
        static auto hamiltonian = new FKGEnergy(potential);

        return hamiltonian;
    }

    RtoR::Function_ptr FKGRtoR_Recipe::getPotential() const {
        if (*PotentialOptions.Potential == MASSLESS_WAVE_EQ) {
            return Slab::New<RtoR::NullFunction>();
        }
        if (*PotentialOptions.Potential == KLEIN_GORDON_POTENTIAL) {
            return Slab::New<RtoR::HarmonicPotential>(*PotentialOptions.MassSquared);
        }
        if (*PotentialOptions.Potential == SIGNUM_GORDON_POTENTIAL) {
            return Slab::New<RtoR::AbsFunction>();
        }
        if (*PotentialOptions.Potential == REGULAR_SG_POTENTIAL) {
            fix m2 = *PotentialOptions.MassSquared;
            fix A = 4/M_PI/m2;
            return Slab::New<RtoR::NonlinearKGPotential>(A, *PotentialOptions.N_num, 1.0);
        }

        throw Exception("Unknown potential");
    }

    TPointer<Base::FunctionT<DevFloat, DevFloat>> FKGRtoR_Recipe::GetNonHomogenousTerm() {
        return {};
    }

    void FKGRtoR_Recipe::SetLaplacianPeriodicBC() {
        Log::Attention() << "KGBuilder Laplacian forced to PERIODIC borders." << Log::Flush;
        force_periodicBC = true;
    }

    void FKGRtoR_Recipe::SetLaplacianFixedBC() {
        // Log::Info() << "KGBuilder Laplacian set to FIXED borders." << Log::Flush;
        force_periodicBC = false;
    }

    bool FKGRtoR_Recipe::usesPeriodicBC() const { return force_periodicBC; }

    Str FKGRtoR_Recipe::SuggestFileName() const {
        const auto StrParams = Interface->ToString({"mass-squared"}, " ");
        const auto ParentSuggestion = FNumericalRecipe::SuggestFileName();

        return ParentSuggestion + " " + StrParams;
    }




}