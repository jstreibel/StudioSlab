#include <catch2/catch_all.hpp>

#include <chrono>
#include <functional>
#include <thread>

#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Scheduling/OutputSchedulerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

namespace {

    using namespace Slab;
    using namespace Slab::Math::Numerics::V2;

    struct FRecordedEventV2 {
        UIntBig Step = 0;
        EEventReasonV2 Reason = EEventReasonV2::Scheduled;
    };

    class FCountingSessionV2 final : public FSimulationSessionV2 {
        FSimulationCursorV2 Cursor;
        std::optional<DevFloat> Dt;

    public:
        explicit FCountingSessionV2(std::optional<DevFloat> dt = std::nullopt)
        : Dt(dt) {
            Cursor.Step = 0;
            Cursor.SimulationTime = dt.has_value() ? std::optional<DevFloat>(0.0) : std::nullopt;
        }

        auto InitializeForCurrentThread() -> void override {
        }

        auto Step(UIntBig nSteps) -> void override {
            Cursor.Step += nSteps;
            if (Dt.has_value() && Cursor.SimulationTime.has_value()) {
                *Cursor.SimulationTime += *Dt * static_cast<DevFloat>(nSteps);
            }
        }

        [[nodiscard]] auto GetCursor() const -> FSimulationCursorV2 override {
            return Cursor;
        }

        [[nodiscard]] auto GetCurrentState() const -> Math::Base::EquationState_constptr override {
            return nullptr;
        }

        [[nodiscard]] auto SupportsSimulationTime() const -> bool override {
            return Dt.has_value();
        }
    };

    class FRecordingListenerV2 : public IListenerV2 {
    protected:
        Vector<FRecordedEventV2> Events;
        Str Name;

        auto Record(const FSimulationEventV2 &event) -> void {
            Events.push_back({event.Cursor.Step, event.Reason});
        }

    public:
        explicit FRecordingListenerV2(Str name = "V2 Recording Listener")
        : Name(std::move(name)) {
        }

        auto OnRunStarted(const FSimulationEventV2 &initialEvent) -> void override {
            Record(initialEvent);
        }

        auto OnSample(const FSimulationEventV2 &event) -> void override {
            Record(event);
        }

        auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool override {
            Record(finalEvent);
            return true;
        }

        [[nodiscard]] auto GetName() const -> Str override {
            return Name;
        }

        [[nodiscard]] auto GetEvents() const -> const Vector<FRecordedEventV2> & {
            return Events;
        }
    };

    class FAbortAtStepListenerV2 final : public FRecordingListenerV2 {
        UIntBig AbortStep;
        FNumericTaskV2 *Task = nullptr;

    public:
        explicit FAbortAtStepListenerV2(UIntBig abortStep)
        : FRecordingListenerV2("V2 Abort Listener")
        , AbortStep(abortStep) {
        }

        auto SetTask(FNumericTaskV2 *task) -> void {
            Task = task;
        }

        auto OnSample(const FSimulationEventV2 &event) -> void override {
            FRecordingListenerV2::OnSample(event);

            if (Task != nullptr && event.Cursor.Step >= AbortStep) {
                Task->Abort();
            }
        }
    };

    class FTestRecipeV2 final : public FSimulationRecipeV2 {
        std::function<TUnique<FSimulationSessionV2>()> SessionBuilder;
        Vector<FSubscriptionV2> Subscriptions;
        FRunLimitsV2 Limits;

    public:
        FTestRecipeV2(std::function<TUnique<FSimulationSessionV2>()> sessionBuilder,
                      Vector<FSubscriptionV2> subscriptions,
                      FRunLimitsV2 limits)
        : SessionBuilder(std::move(sessionBuilder))
        , Subscriptions(std::move(subscriptions))
        , Limits(std::move(limits)) {
        }

        auto BuildSession() -> TUnique<FSimulationSessionV2> override {
            return SessionBuilder();
        }

        auto BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> override {
            return Subscriptions;
        }

        [[nodiscard]] auto GetRunLimits() const -> FRunLimitsV2 override {
            return Limits;
        }
    };

    auto RunTaskAndWait(Core::FTask &task) -> Core::ETaskStatus {
        std::thread worker([&task] { task.Start(); });

        while (true) {
            const auto status = task.GetStatus();
            if (status != Core::TaskNotInitialized && status != Core::TaskRunning) {
                task.Release();
                worker.join();
                return status;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    auto EventSteps(const Vector<FRecordedEventV2> &events) -> Vector<UIntBig> {
        Vector<UIntBig> out;
        out.reserve(events.size());
        for (const auto &e : events) out.push_back(e.Step);
        return out;
    }

    auto EventReasons(const Vector<FRecordedEventV2> &events) -> Vector<EEventReasonV2> {
        Vector<EEventReasonV2> out;
        out.reserve(events.size());
        for (const auto &e : events) out.push_back(e.Reason);
        return out;
    }
}

TEST_CASE("PhaseA V2 - EveryNStepsTrigger reset and monotonic schedule", "[V2][PhaseA][Trigger]") {
    using namespace Slab::Math::Numerics::V2;

    FEveryNStepsTriggerV2 trigger(5);

    FSimulationCursorV2 start{.Step = 12};
    trigger.Reset(start);

    const auto next1 = trigger.GetNextDueStepAfter(start);
    REQUIRE(next1.has_value());
    CHECK(*next1 == 17);

    FSimulationCursorV2 c17{.Step = 17};
    const auto next2 = trigger.GetNextDueStepAfter(c17);
    REQUIRE(next2.has_value());
    CHECK(*next2 == 22);

    CHECK_FALSE(trigger.IsDue(start));
    CHECK(trigger.IsDue(c17));
    CHECK_FALSE(trigger.IsDue(FSimulationCursorV2{.Step = 18}));

    trigger.Reset(FSimulationCursorV2{.Step = 3});
    const auto nextReset = trigger.GetNextDueStepAfter(FSimulationCursorV2{.Step = 3});
    REQUIRE(nextReset.has_value());
    CHECK(*nextReset == 8);
}

TEST_CASE("PhaseA V2 - OutputScheduler chooses earliest due subscriptions", "[V2][PhaseA][Scheduler]") {
    using namespace Slab::Math::Numerics::V2;

    auto listenerA = New<FRecordingListenerV2>("A");
    auto listenerB = New<FRecordingListenerV2>("B");

    Vector<FSubscriptionV2> subscriptions = {
        {New<FEveryNStepsTriggerV2>(10), listenerA, EDeliveryModeV2::Synchronous, false, false},
        {New<FEveryNStepsTriggerV2>(3), listenerB, EDeliveryModeV2::Synchronous, false, false}
    };

    FOutputSchedulerV2 scheduler;
    scheduler.Reset(subscriptions, FSimulationCursorV2{.Step = 0});

    const auto next = scheduler.ComputeNextScheduledStepAfter(subscriptions, FSimulationCursorV2{.Step = 0});
    REQUIRE(next.has_value());
    CHECK(*next == 3);

    const auto dueAt3 = scheduler.CollectDueSubscriptions(subscriptions, FSimulationCursorV2{.Step = 3});
    REQUIRE(dueAt3.size() == 1);
    CHECK(dueAt3[0] == 1);

    const auto dueAt30 = scheduler.CollectDueSubscriptions(subscriptions, FSimulationCursorV2{.Step = 30});
    REQUIRE(dueAt30.size() == 2);
    CHECK(dueAt30[0] == 0);
    CHECK(dueAt30[1] == 1);
}

TEST_CASE("PhaseA V2 - NumericTask finite run dispatches initial scheduled final", "[V2][PhaseA][Task]") {
    using namespace Slab::Math::Numerics::V2;

    auto listener = New<FRecordingListenerV2>("finite");
    Vector<FSubscriptionV2> subscriptions = {
        {New<FEveryNStepsTriggerV2>(3), listener, EDeliveryModeV2::Synchronous, true, true}
    };

    FRunLimitsV2 limits;
    limits.Mode = ERunModeV2::FiniteSteps;
    limits.MaxSteps = 10;

    auto recipe = New<FTestRecipeV2>(
        []() -> TUnique<FSimulationSessionV2> { return std::make_unique<FCountingSessionV2>(); },
        subscriptions,
        limits);

    auto task = New<FNumericTaskV2>(recipe, false, 8);
    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto events = listener->GetEvents();
    CAPTURE(events.size());

    const Vector<UIntBig> expectedSteps = {0, 3, 6, 9, 10};
    const Vector<EEventReasonV2> expectedReasons = {
        EEventReasonV2::Initial,
        EEventReasonV2::Scheduled,
        EEventReasonV2::Scheduled,
        EEventReasonV2::Scheduled,
        EEventReasonV2::Final
    };

    CHECK(EventSteps(events) == expectedSteps);
    CHECK(EventReasons(events) == expectedReasons);

    const auto cursor = task->GetCursor();
    CHECK(cursor.Step == 10);

    const auto progress = task->GetProgress01();
    REQUIRE(progress.has_value());
    CHECK(*progress == Catch::Approx(1.0f).margin(1e-6f));
}

TEST_CASE("PhaseA V2 - NumericTask open-ended runs until abort", "[V2][PhaseA][Task][OpenEnded]") {
    using namespace Slab::Math::Numerics::V2;

    auto listener = New<FAbortAtStepListenerV2>(6);
    Vector<FSubscriptionV2> subscriptions = {
        {New<FEveryNStepsTriggerV2>(2), listener, EDeliveryModeV2::Synchronous, true, true}
    };

    FRunLimitsV2 limits;
    limits.Mode = ERunModeV2::OpenEnded;

    auto recipe = New<FTestRecipeV2>(
        []() -> TUnique<FSimulationSessionV2> { return std::make_unique<FCountingSessionV2>(); },
        subscriptions,
        limits);

    auto task = New<FNumericTaskV2>(recipe, false, 4);
    listener->SetTask(task.get());

    REQUIRE(RunTaskAndWait(*task) == Core::TaskAborted);

    const auto events = listener->GetEvents();
    REQUIRE_FALSE(events.empty());

    const Vector<UIntBig> expectedSteps = {0, 2, 4, 6, 6};
    const Vector<EEventReasonV2> expectedReasons = {
        EEventReasonV2::Initial,
        EEventReasonV2::Scheduled,
        EEventReasonV2::Scheduled,
        EEventReasonV2::Scheduled,
        EEventReasonV2::AbortFinal
    };

    CHECK(EventSteps(events) == expectedSteps);
    CHECK(EventReasons(events) == expectedReasons);

    CHECK_FALSE(task->GetProgress01().has_value());
    CHECK(task->GetCursor().Step == 6);
}
