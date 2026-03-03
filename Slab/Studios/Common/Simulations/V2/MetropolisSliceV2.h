#ifndef STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_METROPOLIS_SLICE_V2_H
#define STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_METROPOLIS_SLICE_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Studios::Common::Simulations::V2 {

    struct FMetropolisExecutionConfigV2 {
        UIntBig Steps = 1000;
        bool bRunEndless = false;
        UIntBig Interval = 1000;
        UIntBig MonitorInterval = 100;
        UIntBig Batch = 2048;
        bool bEnableGLMonitor = false;
    };

    struct FMetropolisMonitorBundleV2 {
        TPointer<Math::Numerics::V2::FSimulationRecipeV2> Recipe = nullptr;
        TPointer<Graphics::FSlabWindow> MonitorWindow = nullptr;
    };

    auto FinalizeMetropolisExecutionConfigV2(FMetropolisExecutionConfigV2 &cfg) -> void;

    auto BuildMetropolisRecipeV2(const FMetropolisExecutionConfigV2 &cfg)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2>;

    auto BuildMetropolisMonitorBundleV2(const FMetropolisExecutionConfigV2 &cfg) -> FMetropolisMonitorBundleV2;

    auto RunMetropolisV2(const FMetropolisExecutionConfigV2 &cfg) -> int;

} // namespace Slab::Studios::Common::Simulations::V2

#endif // STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_METROPOLIS_SLICE_V2_H
