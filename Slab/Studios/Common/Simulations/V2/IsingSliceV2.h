#ifndef STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_ISING_SLICE_V2_H
#define STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_ISING_SLICE_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Data/V2/LiveControlHubV2.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Studios::Common::Simulations::V2 {

    inline constexpr const char *IsingControlTopicPrefixDefaultV2 = "labv2/control/ising";

    struct FIsingExecutionConfigV2 {
        UInt L = 64;
        UIntBig Steps = 2000;
        DevFloat Temperature = 2.269185314;
        DevFloat ExternalField = 0.0;
        bool bFerromagneticInitial = false;

        UIntBig Interval = 100;
        UIntBig MonitorInterval = 20;
        UIntBig Batch = 1024;
        bool bEnableGLMonitor = false;

        bool bEnableLiveParameterBinding = true;
        UIntBig ControlSampleInterval = 1;
        Str ControlTopicPrefix = IsingControlTopicPrefixDefaultV2;
        TPointer<Math::LiveControl::V2::FLiveControlHubV2> ControlHub = nullptr;
    };

    auto FinalizeIsingExecutionConfigV2(FIsingExecutionConfigV2 &cfg) -> void;

    auto BuildIsingRecipeV2(
        const FIsingExecutionConfigV2 &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2>;

    auto BuildIsingPassiveMonitorWindowV2(
        const FIsingExecutionConfigV2 &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
        const TPointer<Math::Numerics::V2::FStateSnapshotListenerV2> &snapshotListener)
        -> TPointer<Graphics::FSlabWindow>;

    auto RunIsingV2(const FIsingExecutionConfigV2 &cfg) -> int;

} // namespace Slab::Studios::Common::Simulations::V2

#endif // STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_ISING_SLICE_V2_H
