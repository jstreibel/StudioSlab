#ifndef STUDIOSLAB_DUMMY_LISTENER_V2_H
#define STUDIOSLAB_DUMMY_LISTENER_V2_H

#include "ListenerV2.h"

namespace Slab::Math::Numerics::V2 {

    class FDummyListenerV2 final : public IListenerV2 {
        Str Name;

    public:
        explicit FDummyListenerV2(Str name = "Dummy Listener V2");

        auto OnSample(const FSimulationEventV2 &event) -> void override;

        [[nodiscard]] auto GetName() const -> Str override;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_DUMMY_LISTENER_V2_H
