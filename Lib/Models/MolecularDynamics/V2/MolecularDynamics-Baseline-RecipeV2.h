#ifndef STUDIOSLAB_MOLECULAR_DYNAMICS_BASELINE_RECIPE_V2_H
#define STUDIOSLAB_MOLECULAR_DYNAMICS_BASELINE_RECIPE_V2_H

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/Stepper.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

#include "Models/MolecularDynamics/MolDynNumericConfig.h"

namespace Slab::Models::MolecularDynamics::V2 {

    enum class EInteractionModelV2 {
        SoftDisk,
        LennardJones
    };

    struct FMolecularDynamicsBaselineConfigV2 {
        UInt N = 256;
        DevFloat L = 50.0;
        DevFloat TotalTime = 50.0;
        UIntBig Steps = 1000;
        DevFloat Temperature = 0.0;
        DevFloat Dissipation = 0.0;
        EInteractionModelV2 InteractionModel = EInteractionModelV2::SoftDisk;
    };

    class FMolecularDynamicsBaselineRecipeV2 final : public Math::Numerics::V2::FSimulationRecipeV2 {
        FMolecularDynamicsBaselineConfigV2 Config;
        UIntBig ConsoleIntervalSteps = 1;
        UIntBig LiveViewIntervalSteps = 0; // 0 => follow ConsoleIntervalSteps
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView = nullptr;
        bool bRunEndless = false;

        auto ValidateConfig() const -> void;
        auto BuildNumericConfig() const -> TPointer<MolDynNumericConfig>;
        auto BuildStepper(const TPointer<MolDynNumericConfig> &numericConfig) const -> TPointer<Math::FStepper>;

    public:
        explicit FMolecularDynamicsBaselineRecipeV2(
            FMolecularDynamicsBaselineConfigV2 config = {},
            UIntBig consoleIntervalSteps = 100,
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr,
            bool bRunEndless = false);

        auto BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> override;
        auto BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> override;

        auto SetLiveViewIntervalSteps(UIntBig liveViewIntervalSteps) -> void;

        [[nodiscard]] auto GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 override;
        [[nodiscard]] auto GetConfig() const -> const FMolecularDynamicsBaselineConfigV2 &;
        [[nodiscard]] auto GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2>;
    };

    DefinePointers(FMolecularDynamicsBaselineRecipeV2)

} // namespace Slab::Models::MolecularDynamics::V2

#endif // STUDIOSLAB_MOLECULAR_DYNAMICS_BASELINE_RECIPE_V2_H
