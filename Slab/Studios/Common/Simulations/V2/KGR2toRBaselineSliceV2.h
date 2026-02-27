#ifndef STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_KG_R2TOR_BASELINE_SLICE_V2_H
#define STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_KG_R2TOR_BASELINE_SLICE_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Models::KGR2toR::Baseline::V2 {
    struct FKGR2toRBaselineConfigV2;
}

namespace Slab::Studios::Common::Simulations::V2 {

    struct FR2toRBaselineExecutionConfig {
        UIntBig Steps = 200;
        DevFloat L = 12.0;
        UInt N = 128;
        DevFloat RDt = 0.1;

        DevFloat XCenter = 0.0;
        DevFloat YCenter = 0.0;
        DevFloat PulseWidth = 0.35;
        DevFloat PhiAmplitude = 0.0;
        DevFloat DPhiDtAmplitude = 1.0;

        UIntBig Interval = 20;
        UIntBig MonitorInterval = 20;
        UIntBig Batch = 2048;
        bool bEnableGLMonitor = false;
    };

    auto FinalizeR2toRBaselineExecutionConfigV2(FR2toRBaselineExecutionConfig &cfg) -> void;

    auto BuildR2toRBaselineRecipeConfigV2(const FR2toRBaselineExecutionConfig &cfg)
        -> Slab::Models::KGR2toR::Baseline::V2::FKGR2toRBaselineConfigV2;

    auto BuildR2toRBaselineRecipeV2(const FR2toRBaselineExecutionConfig &cfg,
                                    const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2>;

    auto BuildR2toRBaselinePassiveMonitorWindowV2(
        const FR2toRBaselineExecutionConfig &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView) -> TPointer<Graphics::FSlabWindow>;

    auto RunR2toRBaselineV2(const FR2toRBaselineExecutionConfig &cfg) -> int;

} // namespace Slab::Studios::Common::Simulations::V2

#endif // STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_KG_R2TOR_BASELINE_SLICE_V2_H
