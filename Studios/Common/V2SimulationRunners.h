#ifndef STUDIOSLAB_V2_SIMULATION_RUNNERS_H
#define STUDIOSLAB_V2_SIMULATION_RUNNERS_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Studios::Common {

    auto RunGLFWMonitoredNumericTaskV2(const Str &windowTitle,
                                       const TPointer<Math::Numerics::V2::FSimulationRecipeV2> &recipe,
                                       const TPointer<Graphics::FSlabWindow> &monitorWindow,
                                       size_t maxBatchSteps = 2048) -> int;

} // namespace Slab::Studios::Common

#endif // STUDIOSLAB_V2_SIMULATION_RUNNERS_H
