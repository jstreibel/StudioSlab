#include "V2SimulationRunners.h"

#include "NumericsV2TaskUtils.h"
#include "VisualHost.h"

#include "Math/Numerics/V2/Task/NumericTaskV2.h"

namespace Slab::Studios::Common {

    auto RunGLFWMonitoredNumericTaskV2(const Str &windowTitle,
                                       const TPointer<Math::Numerics::V2::FSimulationRecipeV2> &recipe,
                                       const TPointer<Graphics::FSlabWindow> &monitorWindow,
                                       const size_t maxBatchSteps) -> int {
        using namespace Slab::Math::Numerics::V2;

        if (recipe == nullptr) throw Exception("V2 monitored numeric task recipe is null.");
        if (monitorWindow == nullptr) throw Exception("V2 monitored numeric task window is null.");

        auto host = CreateGLFWVisualHost(windowTitle);
        AddRootSlabWindow(host, monitorWindow, false);

        auto task = New<FNumericTaskV2>(recipe, false, maxBatchSteps);
        const auto status = RunTaskWithVisualHost(host, *task);
        PrintNumericTaskSummary(*task);
        return ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common
