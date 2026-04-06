#ifndef STUDIOSLAB_LISTENER_V2_H
#define STUDIOSLAB_LISTENER_V2_H

#include "Math/Numerics/V2/Events/SimulationEventV2.h"

namespace Slab::Math::Numerics::V2 {

    class ITriggerV2;

    class IListenerV2 {
    public:
        virtual ~IListenerV2() = default;

        virtual auto OnRunStarted(const FSimulationEventV2 &initialEvent) -> void {
            (void) initialEvent;
        }

        virtual auto OnSample(const FSimulationEventV2 &event) -> void = 0;

        virtual auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool {
            (void) finalEvent;
            return true;
        }

        [[nodiscard]] virtual auto GetName() const -> Str = 0;
    };

    DefinePointers(IListenerV2)

    struct FSubscriptionV2 {
        TPointer<ITriggerV2> Trigger = nullptr;
        TPointer<IListenerV2> Listener = nullptr;
        EDeliveryModeV2 DeliveryMode = EDeliveryModeV2::Synchronous;
        bool bWantsInitialEvent = false;
        bool bWantsFinalEvent = true;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_LISTENER_V2_H
