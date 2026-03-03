#ifndef STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_MOLECULAR_DYNAMICS_SLICE_V2_H
#define STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_MOLECULAR_DYNAMICS_SLICE_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Models::MolecularDynamics::V2 {
    enum class EInteractionModelV2;
}

namespace Slab::Studios::Common::Simulations::V2 {

    enum class EMDInteractionModelV2 {
        SoftDisk,
        LennardJones
    };

    struct FMolecularDynamicsExecutionConfigV2 {
        UInt N = 256;
        DevFloat L = 50.0;
        DevFloat TotalTime = 50.0;
        UIntBig Steps = 1000;
        bool bRunEndless = false;
        DevFloat Temperature = 0.0;
        DevFloat Dissipation = 0.0;
        EMDInteractionModelV2 InteractionModel = EMDInteractionModelV2::SoftDisk;

        UIntBig Interval = 100;
        UIntBig MonitorInterval = 100;
        UIntBig Batch = 512;
        bool bEnableGLMonitor = false;
    };

    auto FinalizeMolecularDynamicsExecutionConfigV2(FMolecularDynamicsExecutionConfigV2 &cfg) -> void;

    auto BuildMolecularDynamicsRecipeV2(
        const FMolecularDynamicsExecutionConfigV2 &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2>;

    auto BuildMolecularDynamicsPassiveMonitorWindowV2(
        const FMolecularDynamicsExecutionConfigV2 &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Graphics::FSlabWindow>;

    auto RunMolecularDynamicsV2(const FMolecularDynamicsExecutionConfigV2 &cfg) -> int;

} // namespace Slab::Studios::Common::Simulations::V2

#endif // STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_MOLECULAR_DYNAMICS_SLICE_V2_H
