#ifndef STUDIOSLAB_KG_RTOR_PLANEWAVES_RECIPE_V2_H
#define STUDIOSLAB_KG_RTOR_PLANEWAVES_RECIPE_V2_H

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/Stepper.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Models::KGRtoR::PlaneWaves::V2 {

    struct FKGRtoRPlaneWavesConfigV2 {
        UInt N = 256;
        DevFloat L = 10.0;
        DevFloat Dt = 0.01;
        UIntBig Steps = 1000;
        DevFloat XCenter = 0.0;
        DevFloat Q = 1.0;
        UInt Harmonic = 1;
    };

    class FKGRtoRPlaneWavesRecipeV2 final : public Math::Numerics::V2::FSimulationRecipeV2 {
        FKGRtoRPlaneWavesConfigV2 Config;
        UIntBig ConsoleIntervalSteps = 1;
        UIntBig LiveViewIntervalSteps = 0; // 0 => follow ConsoleIntervalSteps
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView = nullptr;

        auto ValidateConfig() const -> void;
        auto BuildStepperInstance() const -> TPointer<::Slab::Math::FStepper>;

    public:
        explicit FKGRtoRPlaneWavesRecipeV2(FKGRtoRPlaneWavesConfigV2 config = {},
                                           UIntBig consoleIntervalSteps = 100,
                                           const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr);

        auto BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> override;
        auto BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> override;

        auto SetLiveViewIntervalSteps(UIntBig liveViewIntervalSteps) -> void;

        [[nodiscard]] auto GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 override;
        [[nodiscard]] auto GetConfig() const -> const FKGRtoRPlaneWavesConfigV2 &;
        [[nodiscard]] auto GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2>;
    };

    DefinePointers(FKGRtoRPlaneWavesRecipeV2)

} // namespace Slab::Models::KGRtoR::PlaneWaves::V2

#endif // STUDIOSLAB_KG_RTOR_PLANEWAVES_RECIPE_V2_H
