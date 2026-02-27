#include "MetropolisFieldSnapshotListenerV2.h"

#include "Models/KleinGordon/RtoR-Montecarlo/V2/RtoR-Hamiltonian-MetropolisHastings-RecipeV2.h"

namespace Slab::Studios::Common::Monitors::V2 {

    FMetropolisFieldSnapshotListenerV2::FMetropolisFieldSnapshotListenerV2(
            const TPointer<Models::KGRtoR::Metropolis::V2::FRtoRHamiltonianMetropolisHastingsRecipeV2> &recipe)
    : Recipe(recipe) {
        if (Recipe == nullptr) throw Exception("Metropolis snapshot listener requires a valid recipe.");
    }

    auto FMetropolisFieldSnapshotListenerV2::CaptureSnapshot(
            const Math::Numerics::V2::FSimulationEventV2 &event) -> void {
        const auto phiField = Recipe->GetPhiField();
        const auto piField = Recipe->GetPiField();
        if (phiField == nullptr || piField == nullptr) return;

        phiField->getSpace().syncHost();
        piField->getSpace().syncHost();

        FMetropolisFieldSnapshotV2 snapshot;
        snapshot.Phi = New<Math::RtoR::NumericFunction_CPU>(*phiField);
        snapshot.Pi = New<Math::RtoR::NumericFunction_CPU>(*piField);
        snapshot.Cursor = event.Cursor;
        snapshot.Reason = event.Reason;
        snapshot.Version = event.PublishedVersion;

        std::lock_guard lock(SnapshotMutex);
        LastSnapshot = std::move(snapshot);
    }

    auto FMetropolisFieldSnapshotListenerV2::OnRunStarted(
            const Math::Numerics::V2::FSimulationEventV2 &initialEvent) -> void {
        CaptureSnapshot(initialEvent);
    }

    auto FMetropolisFieldSnapshotListenerV2::OnSample(const Math::Numerics::V2::FSimulationEventV2 &event) -> void {
        CaptureSnapshot(event);
    }

    auto FMetropolisFieldSnapshotListenerV2::OnRunFinished(
            const Math::Numerics::V2::FSimulationEventV2 &finalEvent) -> bool {
        CaptureSnapshot(finalEvent);
        return true;
    }

    auto FMetropolisFieldSnapshotListenerV2::GetName() const -> Str {
        return "Metropolis field snapshot listener V2";
    }

    auto FMetropolisFieldSnapshotListenerV2::TryGetSnapshot() const -> std::optional<FMetropolisFieldSnapshotV2> {
        std::lock_guard lock(SnapshotMutex);
        return LastSnapshot;
    }

} // namespace Slab::Studios::Common::Monitors::V2
