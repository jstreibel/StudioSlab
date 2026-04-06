#ifndef STUDIOSLAB_V2_SIMULATION_RUNNERS_H
#define STUDIOSLAB_V2_SIMULATION_RUNNERS_H

#include "Core/SlabCore.h"

#include "Graphics/Window/SlabWindow.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include <functional>

namespace Slab::Studios::Common {

    using FNumericTaskV2PostRunHook = std::function<void(const Math::Numerics::V2::FNumericTaskV2 &)>;

    auto RunGLFWMonitoredNumericTaskV2(const Str &windowTitle,
                                       const TPointer<Math::Numerics::V2::FSimulationRecipeV2> &recipe,
                                       const TPointer<Graphics::FSlabWindow> &monitorWindow,
                                       size_t maxBatchSteps = 2048,
                                       const FNumericTaskV2PostRunHook &postRunHook = {}) -> int;

} // namespace Slab::Studios::Common

#endif // STUDIOSLAB_V2_SIMULATION_RUNNERS_H
