#include "NumericTaskV2.h"

#include <algorithm>
#include <chrono>

namespace Slab::Math::Numerics::V2 {

    FNumericTaskV2::FNumericTaskV2(const TPointer<FSimulationRecipeV2> &recipe,
                                   const bool preInit,
                                   const size_t maxBatchSteps)
    : FTask("Numeric Integration V2")
    , Recipe(recipe)
    , MaxBatchSteps(std::max<size_t>(1, maxBatchSteps)) {
        if (preInit) Init();
    }

    auto FNumericTaskV2::Init() -> void {
        if (bInitialized) throw Exception("NumericTaskV2 already initialized.");
        if (Recipe == nullptr) throw Exception("NumericTaskV2 requires a valid recipe.");

        Session = Recipe->BuildSession();
        if (Session == nullptr) throw Exception("NumericTaskV2 recipe returned null session.");

        Subscriptions = Recipe->BuildDefaultSubscriptions();
        RunLimits = Recipe->GetRunLimits();

        bInitialized = true;
    }

    auto FNumericTaskV2::IsInitialized() const -> bool {
        return bInitialized && Session != nullptr;
    }

    auto FNumericTaskV2::Abort() -> void {
        bAbortFlag = true;
    }

    auto FNumericTaskV2::GetCursor() const -> FSimulationCursorV2 {
        FSimulationCursorV2 cursor;
        cursor.Step = CachedStep.load(std::memory_order_acquire);
        cursor.WallClockSeconds = CachedWallClockSeconds.load(std::memory_order_acquire);
        if (bCachedHasSimulationTime.load(std::memory_order_acquire)) {
            cursor.SimulationTime = static_cast<DevFloat>(CachedSimulationTime.load(std::memory_order_acquire));
        }
        return cursor;
    }

    auto FNumericTaskV2::GetProgress01() const -> std::optional<float> {
        if (!bCachedHasProgress.load(std::memory_order_acquire)) return std::nullopt;
        return CachedProgress01.load(std::memory_order_acquire);
    }

    auto FNumericTaskV2::GetSession() const -> const FSimulationSessionV2 * {
        return Session.get();
    }

    auto FNumericTaskV2::HasReachedFiniteLimit(const FSimulationCursorV2 &cursor) const -> bool {
        switch (RunLimits.Mode) {
        case ERunModeV2::OpenEnded:
            return false;

        case ERunModeV2::FiniteSteps:
            if (!RunLimits.MaxSteps.has_value()) return true;
            return cursor.Step >= *RunLimits.MaxSteps;

        case ERunModeV2::FiniteSimulationTime:
            if (!RunLimits.MaxSimulationTime.has_value()) return true;
            if (!cursor.SimulationTime.has_value()) return false;
            return *cursor.SimulationTime >= *RunLimits.MaxSimulationTime;
        }

        return false;
    }

    auto FNumericTaskV2::ComputeBatchSteps(const FSimulationCursorV2 &cursor,
                                           const std::optional<UIntBig> &nextDueStep) const -> UIntBig {
        UIntBig batch = static_cast<UIntBig>(MaxBatchSteps);

        if (RunLimits.Mode == ERunModeV2::FiniteSteps && RunLimits.MaxSteps.has_value()) {
            if (cursor.Step >= *RunLimits.MaxSteps) return 0;
            batch = std::min(batch, *RunLimits.MaxSteps - cursor.Step);
        }

        if (nextDueStep.has_value()) {
            if (*nextDueStep <= cursor.Step) return 0;
            batch = std::min(batch, *nextDueStep - cursor.Step);
        }

        return batch;
    }

    auto FNumericTaskV2::BuildEvent(const EEventReasonV2 reason, const bool bRealtimeBestEffort) const -> FSimulationEventV2 {
        FSimulationEventV2 event;
        if (Session != nullptr) {
            auto lease = Session->AcquireReadLease();
            event.Cursor = lease.GetCursor();
            event.State = lease.GetState();
            event.PublishedVersion = lease.GetPublishedVersion();
            event.SessionRef = TPointer<const FSimulationSessionV2>(Session);
        }
        event.Reason = reason;
        event.bIsRealtimeBestEffort = bRealtimeBestEffort;
        return event;
    }

    auto FNumericTaskV2::UpdateCachedTelemetry(const FSimulationCursorV2 &cursor) -> void {
        CachedStep.store(cursor.Step, std::memory_order_release);
        CachedWallClockSeconds.store(cursor.WallClockSeconds, std::memory_order_release);

        const auto simTime = cursor.SimulationTime;
        bCachedHasSimulationTime.store(simTime.has_value(), std::memory_order_release);
        if (simTime.has_value()) {
            CachedSimulationTime.store(*simTime, std::memory_order_release);
        }

        if (RunLimits.Mode == ERunModeV2::FiniteSteps && RunLimits.MaxSteps.has_value()) {
            const auto maxSteps = *RunLimits.MaxSteps;
            const auto progress = maxSteps == 0
                ? 1.0f
                : static_cast<float>(std::min<UIntBig>(cursor.Step, maxSteps)) / static_cast<float>(maxSteps);
            CachedProgress01.store(progress, std::memory_order_release);
            bCachedHasProgress.store(true, std::memory_order_release);
        } else {
            bCachedHasProgress.store(false, std::memory_order_release);
        }
    }

    auto FNumericTaskV2::EmitInitialEventIfRequested() -> void {
        Dispatcher.DispatchRunStarted(Subscriptions, BuildEvent(EEventReasonV2::Initial));
    }

    auto FNumericTaskV2::EmitFinalEvent(const EEventReasonV2 reason) -> bool {
        return Dispatcher.DispatchRunFinished(Subscriptions, BuildEvent(reason));
    }

    auto FNumericTaskV2::Run() -> Core::ETaskStatus {
        if (!IsInitialized()) Init();

        if (Recipe == nullptr || Session == nullptr) return Core::TaskError;

        if (RunLimits.Mode == ERunModeV2::FiniteSimulationTime &&
            !Session->SupportsSimulationTime() &&
            RunLimits.MaxSimulationTime.has_value()) {
            return Core::TaskError;
        }

        Recipe->SetupForCurrentThread();
        Session->InitializeForCurrentThread();

        const auto taskStart = std::chrono::steady_clock::now();

        auto initialCursor = Session->GetCursor();
        initialCursor.WallClockSeconds = 0.0;
        UpdateCachedTelemetry(initialCursor);

        Scheduler.Reset(Subscriptions, initialCursor);
        EmitInitialEventIfRequested();

        while (!bAbortFlag) {
            auto cursor = Session->GetCursor();
            cursor.WallClockSeconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - taskStart).count();
            UpdateCachedTelemetry(cursor);

            if (HasReachedFiniteLimit(cursor)) break;

            const auto nextDue = Scheduler.ComputeNextScheduledStepAfter(Subscriptions, cursor);
            const auto batch = ComputeBatchSteps(cursor, nextDue);

            if (batch == 0) return Core::TaskError;

            Session->Step(batch);

            auto dueCursor = Session->GetCursor();
            dueCursor.WallClockSeconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - taskStart).count();
            UpdateCachedTelemetry(dueCursor);

            const auto due = Scheduler.CollectDueSubscriptionsBetween(Subscriptions, cursor, dueCursor);
            if (!due.empty()) {
                auto event = BuildEvent(EEventReasonV2::Scheduled);
                event.Cursor.WallClockSeconds = dueCursor.WallClockSeconds;
                Dispatcher.DispatchScheduled(Subscriptions, due, event);
            }
        }

        if (bAbortFlag) {
            auto finalCursor = Session->GetCursor();
            finalCursor.WallClockSeconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - taskStart).count();
            UpdateCachedTelemetry(finalCursor);
            EmitFinalEvent(EEventReasonV2::AbortFinal);
            return Core::TaskAborted;
        }

        auto finalCursor = Session->GetCursor();
        finalCursor.WallClockSeconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - taskStart).count();
        UpdateCachedTelemetry(finalCursor);

        if (!EmitFinalEvent(EEventReasonV2::Final)) return Core::TaskError;

        return Core::TaskSuccess;
    }

} // namespace Slab::Math::Numerics::V2
