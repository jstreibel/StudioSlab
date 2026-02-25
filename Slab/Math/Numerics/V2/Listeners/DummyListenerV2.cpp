#include "DummyListenerV2.h"

#include <utility>

namespace Slab::Math::Numerics::V2 {

    FDummyListenerV2::FDummyListenerV2(Str name)
    : Name(std::move(name)) {
    }

    auto FDummyListenerV2::OnSample(const FSimulationEventV2 &event) -> void {
        (void) event;
    }

    auto FDummyListenerV2::GetName() const -> Str {
        return Name;
    }

} // namespace Slab::Math::Numerics::V2
