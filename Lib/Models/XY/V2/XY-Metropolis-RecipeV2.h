#ifndef STUDIOSLAB_XY_METROPOLIS_RECIPE_V2_H
#define STUDIOSLAB_XY_METROPOLIS_RECIPE_V2_H

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

#include <atomic>
#include <numbers>

namespace Slab::Models::XY::V2 {

    struct FXYMetropolisConfigV2 {
        UInt L = 64;
        UIntBig Steps = 2000;
        DevFloat Temperature = 0.7;
        DevFloat ExternalField = 0.0;
        DevFloat DeltaTheta = 2.0 * std::numbers::pi_v<DevFloat>;
        bool bFerromagneticInitial = false;
    };

    class FXYLatticeStateV2 final : public Math::Base::EquationState {
        TPointer<Math::R2toR::NumericFunction_CPU> ThetaField = nullptr;
        DevFloat EnergyDensity = 0.0;
        DevFloat Magnetization = 0.0;
        DevFloat AcceptanceRatio = 0.0;
        UInt AcceptedLastSweep = 0;
        UInt RejectedLastSweep = 0;

        static auto Cast(const Math::Base::EquationState &state) -> const FXYLatticeStateV2 &;
        static auto Cast(Math::Base::EquationState &state) -> FXYLatticeStateV2 &;
        auto EnsureCompatible(const FXYLatticeStateV2 &other) const -> void;

    public:
        explicit FXYLatticeStateV2(const TPointer<Math::R2toR::NumericFunction_CPU> &thetaField);

        auto SetDiagnostics(DevFloat energyDensity,
                            DevFloat magnetization,
                            DevFloat acceptanceRatio,
                            UInt accepted,
                            UInt rejected) -> void;

        [[nodiscard]] auto GetThetaField() const -> TPointer<Math::R2toR::NumericFunction_CPU>;
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

    class FXYRuntimeControlsV2 final {
        std::atomic<DevFloat> Temperature;
        std::atomic<DevFloat> ExternalField;

    public:
        explicit FXYRuntimeControlsV2(const DevFloat temperature = 0.7,
                                      const DevFloat externalField = 0.0)
        : Temperature(temperature)
        , ExternalField(externalField) {
        }

        [[nodiscard]] auto GetTemperature() const -> DevFloat {
            return Temperature.load(std::memory_order_relaxed);
        }

        auto SetTemperature(const DevFloat temperature) -> void {
            Temperature.store(temperature, std::memory_order_relaxed);
        }

        [[nodiscard]] auto GetExternalField() const -> DevFloat {
            return ExternalField.load(std::memory_order_relaxed);
        }

        auto SetExternalField(const DevFloat externalField) -> void {
            ExternalField.store(externalField, std::memory_order_relaxed);
        }
    };

    class FXYMetropolisRecipeV2 final : public Math::Numerics::V2::FSimulationRecipeV2 {
        FXYMetropolisConfigV2 Config;
        TPointer<FXYRuntimeControlsV2> RuntimeControls = nullptr;
        UIntBig ConsoleIntervalSteps = 1;
        UIntBig LiveViewIntervalSteps = 0; // 0 => follow ConsoleIntervalSteps
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView = nullptr;

        auto ValidateConfig() const -> void;

    public:
        explicit FXYMetropolisRecipeV2(
            FXYMetropolisConfigV2 config = {},
            UIntBig consoleIntervalSteps = 100,
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr);

        auto BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> override;
        auto BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> override;

        auto SetLiveViewIntervalSteps(UIntBig liveViewIntervalSteps) -> void;

        [[nodiscard]] auto GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 override;
        [[nodiscard]] auto GetConfig() const -> const FXYMetropolisConfigV2 &;
        [[nodiscard]] auto GetRuntimeControls() const -> TPointer<FXYRuntimeControlsV2>;
        [[nodiscard]] auto GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2>;
    };

    DefinePointers(FXYMetropolisRecipeV2)
    DefinePointers(FXYLatticeStateV2)
    DefinePointers(FXYRuntimeControlsV2)

} // namespace Slab::Models::XY::V2

#endif // STUDIOSLAB_XY_METROPOLIS_RECIPE_V2_H
