#ifndef STUDIOSLAB_SIMULATION_SESSION_V2_H
#define STUDIOSLAB_SIMULATION_SESSION_V2_H

#include "Math/Numerics/V2/Events/SimulationEventV2.h"

#include <atomic>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>

namespace Slab::Math::Numerics::V2 {

    class FSessionReadLeaseV2 {
        std::shared_lock<std::shared_mutex> Lock;
        FSimulationCursorV2 Cursor;
        Base::EquationState_constptr State = nullptr;
        UIntBig PublishedVersion = 0;

    public:
        FSessionReadLeaseV2() = default;

        FSessionReadLeaseV2(std::shared_lock<std::shared_mutex> lock,
                            FSimulationCursorV2 cursor,
                            Base::EquationState_constptr state,
                            const UIntBig publishedVersion)
        : Lock(std::move(lock))
        , Cursor(std::move(cursor))
        , State(std::move(state))
        , PublishedVersion(publishedVersion) {
        }

        FSessionReadLeaseV2(const FSessionReadLeaseV2 &) = delete;
        auto operator=(const FSessionReadLeaseV2 &) -> FSessionReadLeaseV2 & = delete;

        FSessionReadLeaseV2(FSessionReadLeaseV2 &&) noexcept = default;
        auto operator=(FSessionReadLeaseV2 &&) noexcept -> FSessionReadLeaseV2 & = default;

        [[nodiscard]] auto OwnsLock() const -> bool {
            return Lock.owns_lock();
        }

        [[nodiscard]] auto GetCursor() const -> const FSimulationCursorV2 & {
            return Cursor;
        }

        [[nodiscard]] auto GetState() const -> Base::EquationState_constptr {
            return State;
        }

        [[nodiscard]] auto GetPublishedVersion() const -> UIntBig {
            return PublishedVersion;
        }
    };

    class FSimulationSessionV2 {
        mutable std::shared_mutex SessionMutex;
        std::atomic<UIntBig> PublishedVersionCounter = 0;

        [[nodiscard]] auto BuildReadLease(std::shared_lock<std::shared_mutex> lock) const -> FSessionReadLeaseV2 {
            return FSessionReadLeaseV2(
                std::move(lock),
                GetCursorUnsafe(),
                GetCurrentStateUnsafe(),
                PublishedVersionCounter.load(std::memory_order_relaxed));
        }

    public:
        virtual ~FSimulationSessionV2() = default;

        virtual auto InitializeForCurrentThread() -> void = 0;

        auto Step(const UIntBig nSteps) -> void {
            if (nSteps == 0) return;

            std::unique_lock lock(SessionMutex);
            if (StepUnsafe(nSteps)) {
                PublishedVersionCounter.fetch_add(1, std::memory_order_release);
            }
        }

        [[nodiscard]] auto GetCursor() const -> FSimulationCursorV2 {
            std::shared_lock lock(SessionMutex);
            return GetCursorUnsafe();
        }

        [[nodiscard]] auto GetCurrentState() const -> Base::EquationState_constptr {
            std::shared_lock lock(SessionMutex);
            return GetCurrentStateUnsafe();
        }

        [[nodiscard]] auto AcquireReadLease() const -> FSessionReadLeaseV2 {
            std::shared_lock lock(SessionMutex);
            return BuildReadLease(std::move(lock));
        }

        [[nodiscard]] auto TryAcquireReadLease() const -> std::optional<FSessionReadLeaseV2> {
            std::shared_lock lock(SessionMutex, std::try_to_lock);
            if (!lock.owns_lock()) return std::nullopt;

            return BuildReadLease(std::move(lock));
        }

        [[nodiscard]] auto GetPublishedVersion() const -> UIntBig {
            return PublishedVersionCounter.load(std::memory_order_acquire);
        }

        [[nodiscard]] virtual auto SupportsSimulationTime() const -> bool = 0;

    protected:
        virtual auto StepUnsafe(UIntBig nSteps) -> bool = 0;
        [[nodiscard]] virtual auto GetCursorUnsafe() const -> FSimulationCursorV2 = 0;
        [[nodiscard]] virtual auto GetCurrentStateUnsafe() const -> Base::EquationState_constptr = 0;
    };

    DefinePointers(FSimulationSessionV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_SIMULATION_SESSION_V2_H
