#ifndef STUDIOSLAB_ISING_METROPOLIS_RECIPE_V2_H
#define STUDIOSLAB_ISING_METROPOLIS_RECIPE_V2_H

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Models::Ising::V2 {

    struct FIsingMetropolisConfigV2 {
        UInt L = 64;
        UIntBig Steps = 2000;
        DevFloat Temperature = 2.269185314;
        DevFloat ExternalField = 0.0;
        bool bFerromagneticInitial = false;
    };

    class FIsingLatticeStateV2 final : public Math::Base::EquationState {
        TPointer<Math::R2toR::NumericFunction_CPU> SpinField = nullptr;
        DevFloat EnergyDensity = 0.0;
        DevFloat Magnetization = 0.0;
        DevFloat AcceptanceRatio = 0.0;
        UInt AcceptedLastSweep = 0;
        UInt RejectedLastSweep = 0;

        static auto Cast(const Math::Base::EquationState &state) -> const FIsingLatticeStateV2 &;
        static auto Cast(Math::Base::EquationState &state) -> FIsingLatticeStateV2 &;
        auto EnsureCompatible(const FIsingLatticeStateV2 &other) const -> void;

    public:
        explicit FIsingLatticeStateV2(const TPointer<Math::R2toR::NumericFunction_CPU> &spinField);

        auto SetDiagnostics(DevFloat energyDensity,
                            DevFloat magnetization,
                            DevFloat acceptanceRatio,
                            UInt accepted,
                            UInt rejected) -> void;

        [[nodiscard]] auto GetSpinField() const -> TPointer<Math::R2toR::NumericFunction_CPU>;
        [[nodiscard]] auto GetEnergyDensity() const -> DevFloat;
        [[nodiscard]] auto GetMagnetization() const -> DevFloat;
        [[nodiscard]] auto GetAcceptanceRatio() const -> DevFloat;
        [[nodiscard]] auto GetAcceptedLastSweep() const -> UInt;
        [[nodiscard]] auto GetRejectedLastSweep() const -> UInt;

        [[nodiscard]] auto Replicate(std::optional<Str> name) const -> TPointer<Math::Base::EquationState> override;
        [[nodiscard]] auto category() const -> Str override;

        auto setData(const Math::Base::EquationState &state) -> void override;
        auto Add(const Math::Base::EquationState &state) -> Math::Base::EquationState & override;
        auto Subtract(const Math::Base::EquationState &state) -> Math::Base::EquationState & override;
        auto StoreAddition(const Math::Base::EquationState &state1,
                           const Math::Base::EquationState &state2) -> Math::Base::EquationState & override;
        auto StoreSubtraction(const Math::Base::EquationState &state1,
                              const Math::Base::EquationState &state2) -> Math::Base::EquationState & override;
        auto StoreScalarMultiplication(const Math::Base::EquationState &state,
                                       DevFloat a) -> Math::Base::EquationState & override;
        auto Multiply(DevFloat a) -> Math::Base::EquationState & override;
    };

    class FIsingMetropolisRecipeV2 final : public Math::Numerics::V2::FSimulationRecipeV2 {
        FIsingMetropolisConfigV2 Config;
        UIntBig ConsoleIntervalSteps = 1;
        UIntBig LiveViewIntervalSteps = 0; // 0 => follow ConsoleIntervalSteps
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView = nullptr;

        auto ValidateConfig() const -> void;

    public:
        explicit FIsingMetropolisRecipeV2(
            FIsingMetropolisConfigV2 config = {},
            UIntBig consoleIntervalSteps = 100,
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr);

        auto BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> override;
        auto BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> override;

        auto SetLiveViewIntervalSteps(UIntBig liveViewIntervalSteps) -> void;

        [[nodiscard]] auto GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 override;
        [[nodiscard]] auto GetConfig() const -> const FIsingMetropolisConfigV2 &;
        [[nodiscard]] auto GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2>;
    };

    DefinePointers(FIsingMetropolisRecipeV2)
    DefinePointers(FIsingLatticeStateV2)

} // namespace Slab::Models::Ising::V2

#endif // STUDIOSLAB_ISING_METROPOLIS_RECIPE_V2_H
