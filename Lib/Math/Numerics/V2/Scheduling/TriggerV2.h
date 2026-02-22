#ifndef STUDIOSLAB_TRIGGER_V2_H
#define STUDIOSLAB_TRIGGER_V2_H

#include "Math/Numerics/V2/Events/SimulationEventV2.h"

#include <optional>

namespace Slab::Math::Numerics::V2 {

    class ITriggerV2 {
    public:
        virtual ~ITriggerV2() = default;

        virtual auto Reset(const FSimulationCursorV2 &startCursor) -> void = 0;

        [[nodiscard]] virtual auto GetDomain() const -> ETriggerDomainV2 = 0;

        /**
         * Phase A supports step-domain scheduling only.
         * Triggers from other domains should return std::nullopt here.
         */
        [[nodiscard]] virtual auto GetNextDueStepAfter(const FSimulationCursorV2 &currentCursor)
                -> std::optional<UIntBig> = 0;

        [[nodiscard]] virtual auto IsDue(const FSimulationCursorV2 &cursor) const -> bool = 0;
    };

    DefinePointers(ITriggerV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_TRIGGER_V2_H
