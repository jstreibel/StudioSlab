#ifndef STUDIOSLAB_CONSOLE_PROGRESS_LISTENER_V2_H
#define STUDIOSLAB_CONSOLE_PROGRESS_LISTENER_V2_H

#include "ListenerV2.h"

namespace Slab::Math::Numerics::V2 {

    class FConsoleProgressListenerV2 final : public IListenerV2 {
        Str Name;
        std::optional<UIntBig> TotalSteps;

        auto PrintEvent(const FSimulationEventV2 &event, const Str &prefix) const -> void;

    public:
        explicit FConsoleProgressListenerV2(std::optional<UIntBig> totalSteps = std::nullopt,
                                            Str name = "Console Progress V2");

        auto OnRunStarted(const FSimulationEventV2 &initialEvent) -> void override;
        auto OnSample(const FSimulationEventV2 &event) -> void override;
        auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool override;

        [[nodiscard]] auto GetName() const -> Str override;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_CONSOLE_PROGRESS_LISTENER_V2_H
