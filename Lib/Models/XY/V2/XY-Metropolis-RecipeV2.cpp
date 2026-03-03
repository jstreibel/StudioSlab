#include "XY-Metropolis-RecipeV2.h"

#include "Core/SlabCore.h"
#include "Utils/RandUtils.h"

#include "Math/Numerics/Stepper.h"
#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Runtime/StepperSessionV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <utility>

namespace Slab::Models::XY::V2 {

    namespace {

        class FXYMetropolisStepperV2 final : public Math::FStepper {
            FXYMetropolisConfigV2 Config;
            TPointer<FXYRuntimeControlsV2> RuntimeControls = nullptr;
            TPointer<Math::R2toR::NumericFunction_CPU> ThetaField;
            TPointer<FXYLatticeStateV2> State;

            auto InitializeState() -> void {
                if (ThetaField == nullptr) return;

                if (Config.bFerromagneticInitial) {
                    for (UInt j = 0; j < Config.L; ++j) {
                        for (UInt i = 0; i < Config.L; ++i) {
                            ThetaField->At(i, j) = 0.0;
                        }
                    }
                    return;
                }

                const auto twoPi = 2.0 * std::numbers::pi_v<DevFloat>;
                for (UInt j = 0; j < Config.L; ++j) {
                    for (UInt i = 0; i < Config.L; ++i) {
                        ThetaField->At(i, j) = twoPi * RandUtils::RandomUniformReal01();
                    }
                }
            }

            [[nodiscard]] auto DeltaEnergy(const UInt i,
                                           const UInt j,
                                           const DevFloat deltaTheta,
                                           const DevFloat externalField) const -> DevFloat {
                const auto theta0 = ThetaField->At(i, j);
                const auto theta1 = theta0 + deltaTheta;

                const auto north = ThetaField->At(i, (j + Config.L - 1) % Config.L);
                const auto south = ThetaField->At(i, (j + 1) % Config.L);
                const auto east = ThetaField->At((i + 1) % Config.L, j);
                const auto west = ThetaField->At((i + Config.L - 1) % Config.L, j);

                const auto interaction0 =
                    std::cos(theta0 - north) + std::cos(theta0 - south) + std::cos(theta0 - east) + std::cos(theta0 - west);
                const auto interaction1 =
                    std::cos(theta1 - north) + std::cos(theta1 - south) + std::cos(theta1 - east) + std::cos(theta1 - west);

                const auto dInteraction = interaction0 - interaction1;
                const auto dField = externalField * (std::cos(theta0) - std::cos(theta1));
                return dInteraction + dField;
            }

            [[nodiscard]] auto ShouldAccept(const DevFloat deltaEnergy, const DevFloat temperature) const -> bool {
                if (deltaEnergy <= 0.0) return true;
                if (temperature <= 0.0) return false;

                const auto boltzmann = std::exp(-deltaEnergy / temperature);
                return RandUtils::RandomUniformReal01() < std::min<DevFloat>(1.0, boltzmann);
            }

            auto MeasureAndPublishDiagnostics(const UInt accepted,
                                             const UInt rejected,
                                             const DevFloat externalField) -> void {
                const auto latticeSize = static_cast<DevFloat>(Config.L) * static_cast<DevFloat>(Config.L);
                if (latticeSize <= 0.0) {
                    State->SetDiagnostics(0.0, 0.0, 0.0, accepted, rejected);
                    return;
                }

                DevFloat energy = 0.0;
                DevFloat mx = 0.0;
                DevFloat my = 0.0;

                for (UInt j = 0; j < Config.L; ++j) {
                    for (UInt i = 0; i < Config.L; ++i) {
                        const auto theta = ThetaField->At(i, j);
                        const auto east = ThetaField->At((i + 1) % Config.L, j);
                        const auto south = ThetaField->At(i, (j + 1) % Config.L);

                        energy -= std::cos(theta - east) + std::cos(theta - south);
                        energy -= externalField * std::cos(theta);

                        mx += std::cos(theta);
                        my += std::sin(theta);
                    }
                }

                const auto magnetization = std::sqrt(mx * mx + my * my) / latticeSize;
                const auto energyDensity = energy / latticeSize;
                const auto acceptance = static_cast<DevFloat>(accepted) / latticeSize;
                State->SetDiagnostics(energyDensity, magnetization, acceptance, accepted, rejected);
            }

            auto RunOneSweep() -> void {
                UInt accepted = 0;
                UInt rejected = 0;
                const auto externalField = RuntimeControls != nullptr
                    ? RuntimeControls->GetExternalField()
                    : Config.ExternalField;
                const auto temperature = RuntimeControls != nullptr
                    ? RuntimeControls->GetTemperature()
                    : Config.Temperature;

                const auto attempts = Config.L * Config.L;
                for (UInt trial = 0; trial < attempts; ++trial) {
                    const auto i = static_cast<UInt>(RandUtils::RandomUniformUInt() % Config.L);
                    const auto j = static_cast<UInt>(RandUtils::RandomUniformUInt() % Config.L);
                    const auto deltaTheta = Config.DeltaTheta * (RandUtils::RandomUniformReal01() - 0.5);

                    const auto deltaEnergy = DeltaEnergy(i, j, deltaTheta, externalField);
                    if (ShouldAccept(deltaEnergy, temperature)) {
                        ThetaField->At(i, j) += deltaTheta;
                        ++accepted;
                    } else {
                        ++rejected;
                    }
                }

                MeasureAndPublishDiagnostics(accepted, rejected, externalField);
            }

        public:
            explicit FXYMetropolisStepperV2(const FXYMetropolisConfigV2 config,
                                            TPointer<FXYRuntimeControlsV2> runtimeControls)
            : Config(config)
            , RuntimeControls(std::move(runtimeControls)) {
                const auto xMin = -0.5 * static_cast<DevFloat>(Config.L);
                const auto yMin = -0.5 * static_cast<DevFloat>(Config.L);
                ThetaField = New<Math::R2toR::NumericFunction_CPU>(Config.L, Config.L, xMin, yMin, 1.0, 1.0);
                State = New<FXYLatticeStateV2>(ThetaField);
                InitializeState();
                const auto externalField = RuntimeControls != nullptr
                    ? RuntimeControls->GetExternalField()
                    : Config.ExternalField;
                MeasureAndPublishDiagnostics(0, 0, externalField);
            }

            auto Step(const size_t nSteps) -> void override {
                for (size_t step = 0; step < nSteps; ++step) {
                    RunOneSweep();
                }
            }

            [[nodiscard]] auto GetCurrentState() const -> Math::Base::EquationState_constptr override {
                return State;
            }
        };

    } // namespace

    auto FXYLatticeStateV2::Cast(const Math::Base::EquationState &state) -> const FXYLatticeStateV2 & {
        return dynamic_cast<const FXYLatticeStateV2 &>(state);
    }

    auto FXYLatticeStateV2::Cast(Math::Base::EquationState &state) -> FXYLatticeStateV2 & {
        return dynamic_cast<FXYLatticeStateV2 &>(state);
    }

    FXYLatticeStateV2::FXYLatticeStateV2(const TPointer<Math::R2toR::NumericFunction_CPU> &thetaField)
    : ThetaField(thetaField) {
        if (ThetaField == nullptr) throw Exception("FXYLatticeStateV2 requires non-null theta field.");
    }

    auto FXYLatticeStateV2::EnsureCompatible(const FXYLatticeStateV2 &other) const -> void {
        if (ThetaField == nullptr || other.ThetaField == nullptr) {
            throw Exception("FXYLatticeStateV2 is missing theta field.");
        }
        if (ThetaField->getN() != other.ThetaField->getN() || ThetaField->getM() != other.ThetaField->getM()) {
            throw Exception("FXYLatticeStateV2 incompatible lattice dimensions.");
        }
    }

    auto FXYLatticeStateV2::SetDiagnostics(const DevFloat energyDensity,
                                           const DevFloat magnetization,
                                           const DevFloat acceptanceRatio,
                                           const UInt accepted,
                                           const UInt rejected) -> void {
        EnergyDensity = energyDensity;
        Magnetization = magnetization;
        AcceptanceRatio = acceptanceRatio;
        AcceptedLastSweep = accepted;
        RejectedLastSweep = rejected;
    }

    auto FXYLatticeStateV2::GetThetaField() const -> TPointer<Math::R2toR::NumericFunction_CPU> {
        return ThetaField;
    }

    auto FXYLatticeStateV2::GetEnergyDensity() const -> DevFloat {
        return EnergyDensity;
    }

    auto FXYLatticeStateV2::GetMagnetization() const -> DevFloat {
        return Magnetization;
    }

    auto FXYLatticeStateV2::GetAcceptanceRatio() const -> DevFloat {
        return AcceptanceRatio;
    }

    auto FXYLatticeStateV2::GetAcceptedLastSweep() const -> UInt {
        return AcceptedLastSweep;
    }

    auto FXYLatticeStateV2::GetRejectedLastSweep() const -> UInt {
        return RejectedLastSweep;
    }

    auto FXYLatticeStateV2::Replicate(std::optional<Str> name) const -> TPointer<Math::Base::EquationState> {
        auto clonedField = DynamicPointerCast<Math::R2toR::NumericFunction_CPU>(ThetaField->Clone());
        if (clonedField == nullptr) throw Exception("FXYLatticeStateV2 failed to clone theta field.");
        if (name.has_value()) clonedField->change_data_name(*name);

        auto replica = New<FXYLatticeStateV2>(clonedField);
        replica->SetDiagnostics(EnergyDensity, Magnetization, AcceptanceRatio, AcceptedLastSweep, RejectedLastSweep);
        return replica;
    }

    auto FXYLatticeStateV2::category() const -> Str {
        return "mc-xy|R2->R";
    }

    auto FXYLatticeStateV2::setData(const Math::Base::EquationState &state) -> void {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        ThetaField->getSpace().setToValue(other.ThetaField->getSpace());
        EnergyDensity = other.EnergyDensity;
        Magnetization = other.Magnetization;
        AcceptanceRatio = other.AcceptanceRatio;
        AcceptedLastSweep = other.AcceptedLastSweep;
        RejectedLastSweep = other.RejectedLastSweep;
    }

    auto FXYLatticeStateV2::Add(const Math::Base::EquationState &state) -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        ThetaField->Add(*other.ThetaField);
        EnergyDensity += other.EnergyDensity;
        Magnetization += other.Magnetization;
        AcceptanceRatio += other.AcceptanceRatio;
        AcceptedLastSweep += other.AcceptedLastSweep;
        RejectedLastSweep += other.RejectedLastSweep;
        return *this;
    }

    auto FXYLatticeStateV2::Subtract(const Math::Base::EquationState &state) -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        ThetaField->Subtract(*other.ThetaField);
        EnergyDensity -= other.EnergyDensity;
        Magnetization -= other.Magnetization;
        AcceptanceRatio -= other.AcceptanceRatio;
        AcceptedLastSweep = AcceptedLastSweep > other.AcceptedLastSweep ? AcceptedLastSweep - other.AcceptedLastSweep : 0;
        RejectedLastSweep = RejectedLastSweep > other.RejectedLastSweep ? RejectedLastSweep - other.RejectedLastSweep : 0;
        return *this;
    }

    auto FXYLatticeStateV2::StoreAddition(const Math::Base::EquationState &state1,
                                          const Math::Base::EquationState &state2) -> Math::Base::EquationState & {
        const auto &a = Cast(state1);
        const auto &b = Cast(state2);
        a.EnsureCompatible(b);
        EnsureCompatible(a);

        ThetaField->StoreAddition(*a.ThetaField, *b.ThetaField);
        EnergyDensity = a.EnergyDensity + b.EnergyDensity;
        Magnetization = a.Magnetization + b.Magnetization;
        AcceptanceRatio = a.AcceptanceRatio + b.AcceptanceRatio;
        AcceptedLastSweep = a.AcceptedLastSweep + b.AcceptedLastSweep;
        RejectedLastSweep = a.RejectedLastSweep + b.RejectedLastSweep;
        return *this;
    }

    auto FXYLatticeStateV2::StoreSubtraction(const Math::Base::EquationState &state1,
                                             const Math::Base::EquationState &state2) -> Math::Base::EquationState & {
        const auto &a = Cast(state1);
        const auto &b = Cast(state2);
        a.EnsureCompatible(b);
        EnsureCompatible(a);

        ThetaField->StoreSubtraction(*a.ThetaField, *b.ThetaField);
        EnergyDensity = a.EnergyDensity - b.EnergyDensity;
        Magnetization = a.Magnetization - b.Magnetization;
        AcceptanceRatio = a.AcceptanceRatio - b.AcceptanceRatio;
        AcceptedLastSweep = a.AcceptedLastSweep > b.AcceptedLastSweep ? a.AcceptedLastSweep - b.AcceptedLastSweep : 0;
        RejectedLastSweep = a.RejectedLastSweep > b.RejectedLastSweep ? a.RejectedLastSweep - b.RejectedLastSweep : 0;
        return *this;
    }

    auto FXYLatticeStateV2::StoreScalarMultiplication(const Math::Base::EquationState &state,
                                                      const DevFloat a) -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        ThetaField->StoreScalarMultiplication(*other.ThetaField, a);
        EnergyDensity = a * other.EnergyDensity;
        Magnetization = a * other.Magnetization;
        AcceptanceRatio = a * other.AcceptanceRatio;
        AcceptedLastSweep = static_cast<UInt>(std::max<DevFloat>(0.0, std::round(a * static_cast<DevFloat>(other.AcceptedLastSweep))));
        RejectedLastSweep = static_cast<UInt>(std::max<DevFloat>(0.0, std::round(a * static_cast<DevFloat>(other.RejectedLastSweep))));
        return *this;
    }

    auto FXYLatticeStateV2::Multiply(const DevFloat a) -> Math::Base::EquationState & {
        ThetaField->Multiply(a);
        EnergyDensity *= a;
        Magnetization *= a;
        AcceptanceRatio *= a;
        AcceptedLastSweep = static_cast<UInt>(std::max<DevFloat>(0.0, std::round(a * static_cast<DevFloat>(AcceptedLastSweep))));
        RejectedLastSweep = static_cast<UInt>(std::max<DevFloat>(0.0, std::round(a * static_cast<DevFloat>(RejectedLastSweep))));
        return *this;
    }

    FXYMetropolisRecipeV2::FXYMetropolisRecipeV2(FXYMetropolisConfigV2 config,
                                                 const UIntBig consoleIntervalSteps,
                                                 const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
                                                 const bool bInRunEndless)
    : Config(std::move(config))
    , RuntimeControls(New<FXYRuntimeControlsV2>(Config.Temperature, Config.ExternalField))
    , ConsoleIntervalSteps(std::max<UIntBig>(1, consoleIntervalSteps))
    , LiveView(liveView)
    , bRunEndless(bInRunEndless) {
        ValidateConfig();
    }

    auto FXYMetropolisRecipeV2::ValidateConfig() const -> void {
        if (Config.L < 2) throw Exception("XY V2 requires L >= 2.");
        if (Config.Steps == 0) throw Exception("XY V2 requires steps > 0.");
        if (Config.Temperature < 0.0) throw Exception("XY V2 requires temperature >= 0.");
        if (Config.DeltaTheta <= 0.0) throw Exception("XY V2 requires delta-theta > 0.");
    }

    auto FXYMetropolisRecipeV2::BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> {
        ValidateConfig();
        auto stepper = New<FXYMetropolisStepperV2>(Config, RuntimeControls);
        return New<Math::Numerics::V2::FStepperSessionV2>(stepper);
    }

    auto FXYMetropolisRecipeV2::BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> {
        using namespace Slab::Math::Numerics::V2;

        const auto liveViewInterval = LiveViewIntervalSteps > 0 ? LiveViewIntervalSteps : ConsoleIntervalSteps;

        auto console = New<FConsoleProgressListenerV2>(
            bRunEndless ? std::optional<UIntBig>{} : std::optional<UIntBig>{Config.Steps},
            "XY Metropolis Console V2");
        Vector<FSubscriptionV2> subscriptions = {{
            New<FEveryNStepsTriggerV2>(ConsoleIntervalSteps),
            console,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};

        if (LiveView != nullptr) {
            auto publisher = New<FSessionLiveViewPublisherListenerV2>(
                LiveView,
                "XY Live View Publisher V2");
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

    auto FXYMetropolisRecipeV2::SetLiveViewIntervalSteps(const UIntBig liveViewIntervalSteps) -> void {
        LiveViewIntervalSteps = liveViewIntervalSteps == 0
            ? UIntBig(0)
            : std::max<UIntBig>(1, liveViewIntervalSteps);
    }

    auto FXYMetropolisRecipeV2::GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 {
        ValidateConfig();

        Math::Numerics::V2::FRunLimitsV2 limits;
        if (bRunEndless) {
            limits.Mode = Math::Numerics::V2::ERunModeV2::OpenEnded;
            limits.MaxSteps = std::nullopt;
            return limits;
        }
        limits.Mode = Math::Numerics::V2::ERunModeV2::FiniteSteps;
        limits.MaxSteps = Config.Steps;
        return limits;
    }

    auto FXYMetropolisRecipeV2::GetConfig() const -> const FXYMetropolisConfigV2 & {
        return Config;
    }

    auto FXYMetropolisRecipeV2::GetRuntimeControls() const -> TPointer<FXYRuntimeControlsV2> {
        return RuntimeControls;
    }

    auto FXYMetropolisRecipeV2::GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2> {
        return LiveView;
    }

} // namespace Slab::Models::XY::V2
