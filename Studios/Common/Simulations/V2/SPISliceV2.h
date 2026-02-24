#ifndef STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_SPI_SLICE_V2_H
#define STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_SPI_SLICE_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Models::StochasticPathIntegrals {
    class SPINumericConfig;
}

namespace Slab::Studios::Common::Simulations::V2 {

    struct FSPIExecutionConfig {
        UInt Steps = 20;
        DevFloat Dt = 0.05;
        DevFloat Time = 1.0;
        DevFloat L = 1.0;
        UInt N = 64;
        UIntBig Interval = 10;
        UIntBig MonitorInterval = 10;
        UIntBig Batch = 2048;
        bool bEnableGLMonitor = false;
    };

    auto BuildSPINumericConfigV2(const FSPIExecutionConfig &cfg)
        -> TPointer<Slab::Models::StochasticPathIntegrals::SPINumericConfig>;

    auto BuildSPIRecipeV2(const FSPIExecutionConfig &cfg,
                          const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2>;

    auto BuildSPIPassiveMonitorWindowV2(const FSPIExecutionConfig &cfg,
                                        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Graphics::FSlabWindow>;

} // namespace Slab::Studios::Common::Simulations::V2

#endif // STUDIOSLAB_STUDIOS_COMMON_SIMULATIONS_V2_SPI_SLICE_V2_H
