#ifndef STUDIOSLAB_STUDIOS_NUMERICS_V2_TASK_UTILS_H
#define STUDIOSLAB_STUDIOS_NUMERICS_V2_TASK_UTILS_H

#include "VisualHost.h"

#include "Core/Backend/Modules/TaskManager/Task.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

namespace Slab::Studios::Common {

    auto RunTaskAndWait(Core::FTask &task) -> Core::ETaskStatus;

    auto RunTaskWithVisualHost(const FVisualHostContext &host,
                               Core::FTask &task) -> Core::ETaskStatus;

    auto ExitCodeFromTaskStatus(Core::ETaskStatus status) -> int;

    auto PrintNumericTaskSummary(const Math::Numerics::V2::FNumericTaskV2 &task) -> void;

} // namespace Slab::Studios::Common

#endif // STUDIOSLAB_STUDIOS_NUMERICS_V2_TASK_UTILS_H
