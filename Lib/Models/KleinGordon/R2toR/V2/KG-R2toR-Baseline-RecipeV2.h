#ifndef STUDIOSLAB_KG_R2TOR_BASELINE_RECIPE_V2_H
#define STUDIOSLAB_KG_R2TOR_BASELINE_RECIPE_V2_H

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Numerics/Stepper.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Models::KGR2toR::Baseline::V2 {

    struct FKGR2toRBaselineConfigV2 {
        UInt N = 128;
        DevFloat L = 12.0;
        DevFloat RDt = 0.1; // Legacy KG convention: dt = r_dt * h
        UIntBig Steps = 200;

        DevFloat XCenter = 0.0;
        DevFloat YCenter = 0.0;
        DevFloat PulseWidth = 0.35;
        DevFloat PhiAmplitude = 0.0;
        DevFloat DPhiDtAmplitude = 1.0;
    };

    class FKGR2toRBaselineRecipeV2 final : public Math::Numerics::V2::FSimulationRecipeV2 {
        FKGR2toRBaselineConfigV2 Config;
        UIntBig ConsoleIntervalSteps = 1;
        UIntBig LiveViewIntervalSteps = 0; // 0 => follow ConsoleIntervalSteps
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView = nullptr;
        TPointer<Math::R2toR::Function> ExternalSource = nullptr;
        bool bRunEndless = false;

        auto ValidateConfig() const -> void;
        [[nodiscard]] auto ComputeCellSize() const -> DevFloat;
        [[nodiscard]] auto ComputeDt() const -> DevFloat;
        auto BuildStepperInstance() const -> TPointer<::Slab::Math::FStepper>;

    public:
        explicit FKGR2toRBaselineRecipeV2(FKGR2toRBaselineConfigV2 config = {},
                                          UIntBig consoleIntervalSteps = 20,
                                          const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr,
                                          const TPointer<Math::R2toR::Function> &externalSource = nullptr,
                                          bool bRunEndless = false);

        auto BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> override;
        auto BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> override;

        auto SetLiveViewIntervalSteps(UIntBig liveViewIntervalSteps) -> void;

        [[nodiscard]] auto GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 override;
        [[nodiscard]] auto GetConfig() const -> const FKGR2toRBaselineConfigV2 &;
        [[nodiscard]] auto GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2>;
        [[nodiscard]] auto GetExternalSource() const -> TPointer<Math::R2toR::Function>;
        [[nodiscard]] auto GetDt() const -> DevFloat;
    };

    DefinePointers(FKGR2toRBaselineRecipeV2)

} // namespace Slab::Models::KGR2toR::Baseline::V2

#endif // STUDIOSLAB_KG_R2TOR_BASELINE_RECIPE_V2_H
