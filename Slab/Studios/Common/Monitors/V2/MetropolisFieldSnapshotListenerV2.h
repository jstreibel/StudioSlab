#ifndef STUDIOSLAB_METROPOLIS_FIELD_SNAPSHOT_LISTENER_V2_H
#define STUDIOSLAB_METROPOLIS_FIELD_SNAPSHOT_LISTENER_V2_H

#include "Math/Numerics/V2/Listeners/ListenerV2.h"

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

namespace Slab::Models::KGRtoR::Metropolis::V2 {
    class FRtoRHamiltonianMetropolisHastingsRecipeV2;
}

namespace Slab::Studios::Common::Monitors::V2 {

    struct FMetropolisFieldSnapshotV2 {
        TPointer<Math::RtoR::NumericFunction_CPU> Phi = nullptr;
        TPointer<Math::RtoR::NumericFunction_CPU> Pi = nullptr;
        Math::Numerics::V2::FSimulationCursorV2 Cursor{};
        Math::Numerics::V2::EEventReasonV2 Reason = Math::Numerics::V2::EEventReasonV2::Initial;
        UIntBig Version = 0;
    };

    class FMetropolisFieldSnapshotListenerV2 final : public Math::Numerics::V2::IListenerV2 {
        TPointer<Models::KGRtoR::Metropolis::V2::FRtoRHamiltonianMetropolisHastingsRecipeV2> Recipe;
        mutable std::mutex SnapshotMutex;
        std::optional<FMetropolisFieldSnapshotV2> LastSnapshot = std::nullopt;

        auto CaptureSnapshot(const Math::Numerics::V2::FSimulationEventV2 &event) -> void;

    public:
        explicit FMetropolisFieldSnapshotListenerV2(
            const TPointer<Models::KGRtoR::Metropolis::V2::FRtoRHamiltonianMetropolisHastingsRecipeV2> &recipe);

        auto OnRunStarted(const Math::Numerics::V2::FSimulationEventV2 &initialEvent) -> void override;
        auto OnSample(const Math::Numerics::V2::FSimulationEventV2 &event) -> void override;
        auto OnRunFinished(const Math::Numerics::V2::FSimulationEventV2 &finalEvent) -> bool override;

        [[nodiscard]] auto GetName() const -> Str override;
        [[nodiscard]] auto TryGetSnapshot() const -> std::optional<FMetropolisFieldSnapshotV2>;
    };

    DefinePointers(FMetropolisFieldSnapshotListenerV2)

} // namespace Slab::Studios::Common::Monitors::V2

#endif // STUDIOSLAB_METROPOLIS_FIELD_SNAPSHOT_LISTENER_V2_H
