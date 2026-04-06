#include "KG-R2toR-Baseline-RecipeV2.h"

#include "Core/SlabCore.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Math/Function/R2toR/Model/Operators/R2toRLaplacian.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"
#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Runtime/StepperSessionV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include "../BoundaryConditions/R2ToRBoundaryCondition.h"
#include "../EquationState.h"
#include "../KG-R2toRSolver.h"

#include <algorithm>

namespace Slab::Models::KGR2toR::Baseline::V2 {

    using namespace Slab::Math;
    using namespace Slab::Math::Numerics::V2;

    namespace {

        class FCenteredRegularDeltaV2 final : public R2toR::Function {
            R2toR::R2toRRegularDelta Delta;
            Real2D Center;

        public:
            FCenteredRegularDeltaV2(const DevFloat eps,
                                   const DevFloat amplitude,
                                   const Real2D center)
            : Delta(eps, amplitude)
            , Center(center) {
            }

            auto operator()(Real2D x) const -> DevFloat override {
                x = x - Center;
                return Delta(x);
            }
        };

        class FInitialPulseBoundaryConditionV2 final : public R2toR::FBoundaryCondition {
            R2toR::Function_ptr InitialPhi;
            R2toR::Function_ptr InitialDPhiDt;

        public:
            FInitialPulseBoundaryConditionV2(const R2toR::EquationState_constptr &prototype,
                                            const R2toR::Function_ptr &initialPhi,
                                            const R2toR::Function_ptr &initialDPhiDt)
            : R2toR::FBoundaryCondition(initialPhi.get(), initialDPhiDt.get(), prototype)
            , InitialPhi(initialPhi)
            , InitialDPhiDt(initialDPhiDt) {
            }
        };

        auto MakeField(const FKGR2toRBaselineConfigV2 &cfg) -> TPointer<R2toR::NumericFunction_CPU> {
            const auto h = cfg.L / static_cast<DevFloat>(cfg.N);
            const auto xMin = cfg.XCenter - cfg.L * static_cast<DevFloat>(0.5);
            const auto yMin = cfg.YCenter - cfg.L * static_cast<DevFloat>(0.5);
            return New<R2toR::NumericFunction_CPU>(cfg.N, cfg.N, xMin, yMin, h, h);
        }

        auto MakeInitialFunction(const DevFloat amplitude,
                                 const DevFloat eps,
                                 const Real2D center) -> R2toR::Function_ptr {
            if (Common::AreEqual(amplitude, 0.0)) return New<R2toR::NullFunction>();
            return New<FCenteredRegularDeltaV2>(eps, amplitude, center);
        }

    } // namespace

    FKGR2toRBaselineRecipeV2::FKGR2toRBaselineRecipeV2(FKGR2toRBaselineConfigV2 config,
                                                       const UIntBig consoleIntervalSteps,
                                                       const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
                                                       const TPointer<Math::R2toR::Function> &externalSource,
                                                       const bool bInRunEndless)
    : Config(std::move(config))
    , ConsoleIntervalSteps(std::max<UIntBig>(1, consoleIntervalSteps))
    , LiveView(liveView)
    , ExternalSource(externalSource)
    , bRunEndless(bInRunEndless) {
        ValidateConfig();
    }

    auto FKGR2toRBaselineRecipeV2::ValidateConfig() const -> void {
        if (Config.N < 8) throw Exception("KGR2toR Baseline V2 requires N >= 8.");
        if (Config.L <= 0.0) throw Exception("KGR2toR Baseline V2 requires L > 0.");
        if (Config.RDt <= 0.0) throw Exception("KGR2toR Baseline V2 requires r_dt > 0.");
        if (Config.Steps == 0) throw Exception("KGR2toR Baseline V2 requires steps > 0.");
        if (Config.PulseWidth <= 0.0) throw Exception("KGR2toR Baseline V2 requires pulse width > 0.");
    }

    auto FKGR2toRBaselineRecipeV2::ComputeCellSize() const -> DevFloat {
        ValidateConfig();
        return Config.L / static_cast<DevFloat>(Config.N);
    }

    auto FKGR2toRBaselineRecipeV2::ComputeDt() const -> DevFloat {
        return Config.RDt * ComputeCellSize();
    }

    auto FKGR2toRBaselineRecipeV2::BuildStepperInstance() const -> TPointer<::Slab::Math::FStepper> {
        ValidateConfig();

        auto phi = MakeField(Config);
        auto dPhiDt = MakeField(Config);
        auto prototypeState = New<R2toR::EquationState>(phi, dPhiDt);

        const Real2D center{Config.XCenter, Config.YCenter};
        auto initialPhi = MakeInitialFunction(Config.PhiAmplitude, Config.PulseWidth, center);
        auto initialDPhiDt = MakeInitialFunction(Config.DPhiDtAmplitude, Config.PulseWidth, center);
        auto boundary = New<FInitialPulseBoundaryConditionV2>(prototypeState, initialPhi, initialDPhiDt);

        auto laplacian = New<Math::R2toR::R2toRLaplacian>();
        auto potential = New<RtoR::AbsFunction>();
        auto solver = New<KGR2toRSolver>(boundary, laplacian, potential, ExternalSource);

        return New<FRungeKutta4>(solver, ComputeDt());
    }

    auto FKGR2toRBaselineRecipeV2::BuildSession() -> TPointer<FSimulationSessionV2> {
        const auto dt = ComputeDt();
        return New<FStepperSessionV2>(BuildStepperInstance(), dt, 0, DevFloat(0.0));
    }

    auto FKGR2toRBaselineRecipeV2::BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> {
        const auto liveViewInterval = LiveViewIntervalSteps > 0 ? LiveViewIntervalSteps : ConsoleIntervalSteps;

        auto console = New<FConsoleProgressListenerV2>(
            bRunEndless ? std::optional<UIntBig>{} : std::optional<UIntBig>{Config.Steps},
            "KGR2toR Baseline Console V2");
        Vector<FSubscriptionV2> subscriptions = {{
            New<FEveryNStepsTriggerV2>(ConsoleIntervalSteps),
            console,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};

        if (LiveView != nullptr) {
            auto publisher = New<FSessionLiveViewPublisherListenerV2>(LiveView, "KGR2toR Baseline Live View Publisher V2");
            subscriptions.push_back({
                New<FEveryNStepsTriggerV2>(liveViewInterval),
                publisher,
                EDeliveryModeV2::Synchronous,
                true,
                true
            });
        }

        return subscriptions;
    }

    auto FKGR2toRBaselineRecipeV2::SetLiveViewIntervalSteps(const UIntBig liveViewIntervalSteps) -> void {
        LiveViewIntervalSteps = liveViewIntervalSteps == 0 ? UIntBig(0) : std::max<UIntBig>(1, liveViewIntervalSteps);
    }

    auto FKGR2toRBaselineRecipeV2::GetRunLimits() const -> FRunLimitsV2 {
        ValidateConfig();
        FRunLimitsV2 limits;
        if (bRunEndless) {
            limits.Mode = ERunModeV2::OpenEnded;
            limits.MaxSteps = std::nullopt;
            return limits;
        }
        limits.Mode = ERunModeV2::FiniteSteps;
        limits.MaxSteps = Config.Steps;
        return limits;
    }

    auto FKGR2toRBaselineRecipeV2::GetConfig() const -> const FKGR2toRBaselineConfigV2 & {
        return Config;
    }

    auto FKGR2toRBaselineRecipeV2::GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2> {
        return LiveView;
    }

    auto FKGR2toRBaselineRecipeV2::GetExternalSource() const -> TPointer<Math::R2toR::Function> {
        return ExternalSource;
    }

    auto FKGR2toRBaselineRecipeV2::GetDt() const -> DevFloat {
        return ComputeDt();
    }

} // namespace Slab::Models::KGR2toR::Baseline::V2
