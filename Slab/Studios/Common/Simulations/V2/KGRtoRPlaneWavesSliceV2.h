#ifndef STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_KG_RTOR_PLANEWAVES_SLICE_V2_H
#define STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_KG_RTOR_PLANEWAVES_SLICE_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

#include <optional>

namespace Slab::Studios::Common::Simulations::V2 {

    enum class ERtoRInitialConditionV2 {
        PlaneWave,
        PerturbedOscillon
    };

    struct FRtoRExecutionConfig {
        UIntBig Steps = 200;
        bool bRunEndless = false;
        DevFloat Dt = -1.0;
        DevFloat L = 10.0;
        UInt N = 256;
        DevFloat XCenter = 0.0;
        DevFloat Q = 1.0;
        UInt Harmonic = 2;
        DevFloat Lambda = 1.0;
        DevFloat Epsilon = 1.0;
        ERtoRInitialConditionV2 InitialCondition = ERtoRInitialConditionV2::PlaneWave;
        UIntBig Interval = 20;
        UIntBig MonitorInterval = 20;
        UIntBig DFTInterval = 0; // 0 => follow Interval
        UIntBig Batch = 2048;
        bool bEnableGLMonitor = false;
        bool bHistorySummary = false;
        bool bSnapshotSummary = false;
        std::optional<UInt> DFTProbeIndex = std::nullopt;
    };

    auto FinalizeRtoRExecutionConfigV2(FRtoRExecutionConfig &cfg) -> void;

    auto BuildRtoRRecipeV2(const FRtoRExecutionConfig &cfg,
                           const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2>;

    auto BuildRtoRPassiveMonitorWindowV2(
        const FRtoRExecutionConfig &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView) -> TPointer<Graphics::FSlabWindow>;

    auto RunRtoRV2(const FRtoRExecutionConfig &cfg) -> int;

} // namespace Slab::Studios::Common::Simulations::V2

#endif // STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_KG_RTOR_PLANEWAVES_SLICE_V2_H
