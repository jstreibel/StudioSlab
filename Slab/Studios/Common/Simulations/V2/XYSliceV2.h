#ifndef STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_XY_SLICE_V2_H
#define STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_XY_SLICE_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

#include <numbers>

namespace Slab::Studios::Common::Simulations::V2 {

    struct FXYExecutionConfigV2 {
        UInt L = 64;
        UIntBig Steps = 2000;
        DevFloat Temperature = 0.7;
        DevFloat ExternalField = 0.0;
        DevFloat DeltaTheta = 2.0 * std::numbers::pi_v<DevFloat>;
        bool bFerromagneticInitial = false;

        UIntBig Interval = 100;
        UIntBig MonitorInterval = 20;
        UIntBig Batch = 1024;
        bool bEnableGLMonitor = false;
    };

    auto FinalizeXYExecutionConfigV2(FXYExecutionConfigV2 &cfg) -> void;

    auto BuildXYRecipeV2(
        const FXYExecutionConfigV2 &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2>;

    auto BuildXYPassiveMonitorWindowV2(
        const FXYExecutionConfigV2 &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Graphics::FSlabWindow>;

    auto RunXYV2(const FXYExecutionConfigV2 &cfg) -> int;

} // namespace Slab::Studios::Common::Simulations::V2

#endif // STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_XY_SLICE_V2_H
