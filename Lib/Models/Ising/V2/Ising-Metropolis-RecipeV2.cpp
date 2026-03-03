#include "Ising-Metropolis-RecipeV2.h"

#include "Core/SlabCore.h"
#include "Utils/RandUtils.h"

#include "Math/Numerics/Stepper.h"
#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Runtime/StepperSessionV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace Slab::Models::Ising::V2 {

    namespace {

        class FIsingMetropolisStepperV2 final : public Math::FStepper {
            FIsingMetropolisConfigV2 Config;
            TPointer<FIsingRuntimeControlsV2> RuntimeControls = nullptr;
            TPointer<Math::R2toR::NumericFunction_CPU> SpinField;
            TPointer<FIsingLatticeStateV2> State;

            auto InitializeState() -> void {
                if (SpinField == nullptr) return;

                if (Config.bFerromagneticInitial) {
                    for (UInt j = 0; j < Config.L; ++j) {
                        for (UInt i = 0; i < Config.L; ++i) {
                            SpinField->At(i, j) = 1.0;
                        }
                    }
                    return;
                }

                for (UInt j = 0; j < Config.L; ++j) {
                    for (UInt i = 0; i < Config.L; ++i) {
                        SpinField->At(i, j) = RandUtils::RandomUniformReal01() < 0.5 ? -1.0 : 1.0;
                    }
                }
            }

            [[nodiscard]] auto DeltaEnergy(const UInt i,
                                           const UInt j,
                                           const DevFloat externalField) const -> DevFloat {
                const auto s = SpinField->At(i, j);
                const auto north = SpinField->At(i, (j + Config.L - 1) % Config.L);
                const auto south = SpinField->At(i, (j + 1) % Config.L);
                const auto east = SpinField->At((i + 1) % Config.L, j);
                const auto west = SpinField->At((i + Config.L - 1) % Config.L, j);
                const auto neighborSum = north + south + east + west;

                return 2.0 * s * (neighborSum + externalField);
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
                DevFloat magnetization = 0.0;

                for (UInt j = 0; j < Config.L; ++j) {
                    for (UInt i = 0; i < Config.L; ++i) {
                        const auto s = SpinField->At(i, j);
                        const auto east = SpinField->At((i + 1) % Config.L, j);
                        const auto south = SpinField->At(i, (j + 1) % Config.L);

                        energy -= s * (east + south);
                        energy -= externalField * s;
                        magnetization += s;
                    }
                }

                const auto energyDensity = energy / latticeSize;
                const auto magDensity = magnetization / latticeSize;
                const auto acceptance = static_cast<DevFloat>(accepted) / latticeSize;
                State->SetDiagnostics(energyDensity, magDensity, acceptance, accepted, rejected);
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

                    const auto deltaEnergy = DeltaEnergy(i, j, externalField);
                    if (ShouldAccept(deltaEnergy, temperature)) {
                        SpinField->At(i, j) = -SpinField->At(i, j);
                        ++accepted;
                    } else {
                        ++rejected;
                    }
                }

                MeasureAndPublishDiagnostics(accepted, rejected, externalField);
            }

        public:
            explicit FIsingMetropolisStepperV2(const FIsingMetropolisConfigV2 config,
                                               TPointer<FIsingRuntimeControlsV2> runtimeControls)
            : Config(config)
            , RuntimeControls(std::move(runtimeControls)) {
                const auto xMin = -0.5 * static_cast<DevFloat>(Config.L);
                const auto yMin = -0.5 * static_cast<DevFloat>(Config.L);
                SpinField = New<Math::R2toR::NumericFunction_CPU>(Config.L, Config.L, xMin, yMin, 1.0, 1.0);
                State = New<FIsingLatticeStateV2>(SpinField);
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

    auto FIsingLatticeStateV2::Cast(const Math::Base::EquationState &state) -> const FIsingLatticeStateV2 & {
        return dynamic_cast<const FIsingLatticeStateV2 &>(state);
    }

    auto FIsingLatticeStateV2::Cast(Math::Base::EquationState &state) -> FIsingLatticeStateV2 & {
        return dynamic_cast<FIsingLatticeStateV2 &>(state);
    }

    FIsingLatticeStateV2::FIsingLatticeStateV2(const TPointer<Math::R2toR::NumericFunction_CPU> &spinField)
    : SpinField(spinField) {
        if (SpinField == nullptr) throw Exception("FIsingLatticeStateV2 requires non-null spin field.");
    }

    auto FIsingLatticeStateV2::EnsureCompatible(const FIsingLatticeStateV2 &other) const -> void {
        if (SpinField == nullptr || other.SpinField == nullptr) {
            throw Exception("FIsingLatticeStateV2 is missing spin field.");
        }
        if (SpinField->getN() != other.SpinField->getN() || SpinField->getM() != other.SpinField->getM()) {
            throw Exception("FIsingLatticeStateV2 incompatible lattice dimensions.");
        }
    }

    auto FIsingLatticeStateV2::SetDiagnostics(const DevFloat energyDensity,
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

    auto FIsingLatticeStateV2::GetSpinField() const -> TPointer<Math::R2toR::NumericFunction_CPU> {
        return SpinField;
    }

    auto FIsingLatticeStateV2::GetEnergyDensity() const -> DevFloat {
        return EnergyDensity;
    }

    auto FIsingLatticeStateV2::GetMagnetization() const -> DevFloat {
        return Magnetization;
    }

    auto FIsingLatticeStateV2::GetAcceptanceRatio() const -> DevFloat {
        return AcceptanceRatio;
    }

    auto FIsingLatticeStateV2::GetAcceptedLastSweep() const -> UInt {
        return AcceptedLastSweep;
    }

    auto FIsingLatticeStateV2::GetRejectedLastSweep() const -> UInt {
        return RejectedLastSweep;
    }

    auto FIsingLatticeStateV2::Replicate(std::optional<Str> name) const -> TPointer<Math::Base::EquationState> {
        auto clonedField = DynamicPointerCast<Math::R2toR::NumericFunction_CPU>(SpinField->Clone());
        if (clonedField == nullptr) throw Exception("FIsingLatticeStateV2 failed to clone spin field.");
        if (name.has_value()) clonedField->change_data_name(*name);

        auto replica = New<FIsingLatticeStateV2>(clonedField);
        replica->SetDiagnostics(EnergyDensity, Magnetization, AcceptanceRatio, AcceptedLastSweep, RejectedLastSweep);
        return replica;
    }

    auto FIsingLatticeStateV2::category() const -> Str {
        return "mc-ising|R2->R";
    }

    auto FIsingLatticeStateV2::setData(const Math::Base::EquationState &state) -> void {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        SpinField->getSpace().setToValue(other.SpinField->getSpace());
        EnergyDensity = other.EnergyDensity;
        Magnetization = other.Magnetization;
        AcceptanceRatio = other.AcceptanceRatio;
        AcceptedLastSweep = other.AcceptedLastSweep;
        RejectedLastSweep = other.RejectedLastSweep;
    }

    auto FIsingLatticeStateV2::Add(const Math::Base::EquationState &state) -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        SpinField->Add(*other.SpinField);
        EnergyDensity += other.EnergyDensity;
        Magnetization += other.Magnetization;
        AcceptanceRatio += other.AcceptanceRatio;
        AcceptedLastSweep += other.AcceptedLastSweep;
        RejectedLastSweep += other.RejectedLastSweep;
        return *this;
    }

    auto FIsingLatticeStateV2::Subtract(const Math::Base::EquationState &state) -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        SpinField->Subtract(*other.SpinField);
        EnergyDensity -= other.EnergyDensity;
        Magnetization -= other.Magnetization;
        AcceptanceRatio -= other.AcceptanceRatio;
        AcceptedLastSweep = AcceptedLastSweep > other.AcceptedLastSweep ? AcceptedLastSweep - other.AcceptedLastSweep : 0;
        RejectedLastSweep = RejectedLastSweep > other.RejectedLastSweep ? RejectedLastSweep - other.RejectedLastSweep : 0;
        return *this;
    }

    auto FIsingLatticeStateV2::StoreAddition(const Math::Base::EquationState &state1,
                                             const Math::Base::EquationState &state2) -> Math::Base::EquationState & {
        const auto &a = Cast(state1);
        const auto &b = Cast(state2);
        a.EnsureCompatible(b);
        EnsureCompatible(a);

        SpinField->StoreAddition(*a.SpinField, *b.SpinField);
        EnergyDensity = a.EnergyDensity + b.EnergyDensity;
        Magnetization = a.Magnetization + b.Magnetization;
        AcceptanceRatio = a.AcceptanceRatio + b.AcceptanceRatio;
        AcceptedLastSweep = a.AcceptedLastSweep + b.AcceptedLastSweep;
        RejectedLastSweep = a.RejectedLastSweep + b.RejectedLastSweep;
        return *this;
    }

    auto FIsingLatticeStateV2::StoreSubtraction(const Math::Base::EquationState &state1,
                                                const Math::Base::EquationState &state2) -> Math::Base::EquationState & {
        const auto &a = Cast(state1);
        const auto &b = Cast(state2);
        a.EnsureCompatible(b);
        EnsureCompatible(a);

        SpinField->StoreSubtraction(*a.SpinField, *b.SpinField);
        EnergyDensity = a.EnergyDensity - b.EnergyDensity;
        Magnetization = a.Magnetization - b.Magnetization;
        AcceptanceRatio = a.AcceptanceRatio - b.AcceptanceRatio;
        AcceptedLastSweep = a.AcceptedLastSweep > b.AcceptedLastSweep ? a.AcceptedLastSweep - b.AcceptedLastSweep : 0;
        RejectedLastSweep = a.RejectedLastSweep > b.RejectedLastSweep ? a.RejectedLastSweep - b.RejectedLastSweep : 0;
        return *this;
    }

    auto FIsingLatticeStateV2::StoreScalarMultiplication(const Math::Base::EquationState &state,
                                                         const DevFloat a) -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        SpinField->StoreScalarMultiplication(*other.SpinField, a);
        EnergyDensity = a * other.EnergyDensity;
        Magnetization = a * other.Magnetization;
        AcceptanceRatio = a * other.AcceptanceRatio;
        AcceptedLastSweep = static_cast<UInt>(std::max<DevFloat>(0.0, std::round(a * static_cast<DevFloat>(other.AcceptedLastSweep))));
        RejectedLastSweep = static_cast<UInt>(std::max<DevFloat>(0.0, std::round(a * static_cast<DevFloat>(other.RejectedLastSweep))));
        return *this;
    }

    auto FIsingLatticeStateV2::Multiply(const DevFloat a) -> Math::Base::EquationState & {
        SpinField->Multiply(a);
        EnergyDensity *= a;
        Magnetization *= a;
        AcceptanceRatio *= a;
        AcceptedLastSweep = static_cast<UInt>(std::max<DevFloat>(0.0, std::round(a * static_cast<DevFloat>(AcceptedLastSweep))));
        RejectedLastSweep = static_cast<UInt>(std::max<DevFloat>(0.0, std::round(a * static_cast<DevFloat>(RejectedLastSweep))));
        return *this;
    }

    FIsingMetropolisRecipeV2::FIsingMetropolisRecipeV2(FIsingMetropolisConfigV2 config,
                                                       const UIntBig consoleIntervalSteps,
                                                       const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
                                                       const bool bInRunEndless)
    : Config(std::move(config))
    , RuntimeControls(New<FIsingRuntimeControlsV2>(Config.Temperature, Config.ExternalField))
    , ConsoleIntervalSteps(std::max<UIntBig>(1, consoleIntervalSteps))
    , LiveView(liveView)
    , bRunEndless(bInRunEndless) {
        ValidateConfig();
    }

    auto FIsingMetropolisRecipeV2::ValidateConfig() const -> void {
        if (Config.L < 2) throw Exception("Ising V2 requires L >= 2.");
        if (Config.Steps == 0) throw Exception("Ising V2 requires steps > 0.");
        if (Config.Temperature < 0.0) throw Exception("Ising V2 requires temperature >= 0.");
    }

    auto FIsingMetropolisRecipeV2::BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> {
        ValidateConfig();
        auto stepper = New<FIsingMetropolisStepperV2>(Config, RuntimeControls);
        return New<Math::Numerics::V2::FStepperSessionV2>(stepper);
    }

    auto FIsingMetropolisRecipeV2::BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> {
        using namespace Slab::Math::Numerics::V2;

        const auto liveViewInterval = LiveViewIntervalSteps > 0 ? LiveViewIntervalSteps : ConsoleIntervalSteps;

        auto console = New<FConsoleProgressListenerV2>(
            bRunEndless ? std::optional<UIntBig>{} : std::optional<UIntBig>{Config.Steps},
            "Ising Metropolis Console V2");
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
                "Ising Live View Publisher V2");
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

    auto FIsingMetropolisRecipeV2::SetLiveViewIntervalSteps(const UIntBig liveViewIntervalSteps) -> void {
        LiveViewIntervalSteps = liveViewIntervalSteps == 0
            ? UIntBig(0)
            : std::max<UIntBig>(1, liveViewIntervalSteps);
    }

    auto FIsingMetropolisRecipeV2::GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 {
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

    auto FIsingMetropolisRecipeV2::GetConfig() const -> const FIsingMetropolisConfigV2 & {
        return Config;
    }

    auto FIsingMetropolisRecipeV2::GetRuntimeControls() const -> TPointer<FIsingRuntimeControlsV2> {
        return RuntimeControls;
    }

    auto FIsingMetropolisRecipeV2::GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2> {
        return LiveView;
    }

} // namespace Slab::Models::Ising::V2
