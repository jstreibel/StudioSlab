#ifndef STUDIOSLAB_STUDIOS_SESSION_LIVE_VIEW_STATS_V2_H
#define STUDIOSLAB_STUDIOS_SESSION_LIVE_VIEW_STATS_V2_H

#include "Graphics/Window/GUIWindow.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Events/SimulationEventV2.h"

#include <optional>

namespace Slab::Studios::Common {

    [[nodiscard]] auto ToDisplayString(Math::Numerics::V2::EEventReasonV2 reason) -> Str;
    [[nodiscard]] auto ToDisplayString(Math::LiveData::V2::ESessionRunStateV2 runState) -> Str;

    auto AppendSessionLiveViewStats(
        const TPointer<Graphics::FGUIWindow> &guiWindow,
        const Str &title,
        const std::optional<Math::LiveData::V2::FSessionTelemetryV2> &telemetry,
        bool bHasBoundSession,
        bool bLeaseAcquired,
        UIntBig maxSteps,
        const std::optional<Str> &extraLine = std::nullopt) -> void;

} // namespace Slab::Studios::Common

#endif // STUDIOSLAB_STUDIOS_SESSION_LIVE_VIEW_STATS_V2_H
