#include <catch2/catch_all.hpp>

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <thread>

#include "Utils/RandUtils.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Data/V2/LiveDataHubV2.h"
#include "Math/Data/V2/LiveControlHubV2.h"
#include "Math/Data/V2/LiveControlTopicsV2.h"
#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
#include "Math/Numerics/V2/Listeners/CursorHistoryListenerV2.h"
#include "Math/Numerics/V2/Listeners/DummyListenerV2.h"
#include "Math/Numerics/V2/Listeners/ScalarTimeDFTListenerV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Scheduling/EveryWallClockTriggerV2.h"
#include "Math/Numerics/V2/Scheduling/OutputSchedulerV2.h"
#include "Math/Numerics/V2/Scheduling/WindowedEveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"
#include "Models/KleinGordon/RtoR/LinearStepping/V2/KG-RtoR-PlaneWaves-RecipeV2.h"
#include "Models/KleinGordon/R2toR/V2/KG-R2toR-Baseline-RecipeV2.h"
#include "Models/KleinGordon/RtoR-Montecarlo/V2/RtoR-Hamiltonian-MetropolisHastings-RecipeV2.h"
#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"
#include "Models/Stochastic-Path-Integral/SPI-State.h"
#include "Models/Stochastic-Path-Integral/V2/SPI-RecipeV2.h"

namespace {

    using namespace Slab;
    using namespace Slab::Math::Numerics::V2;

    struct FRecordedEventV2 {
        UIntBig Step = 0;
        EEventReasonV2 Reason = EEventReasonV2::Scheduled;
        std::optional<DevFloat> SimulationTime = std::nullopt;
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

    protected:
        auto StepUnsafe(UIntBig nSteps) -> bool override {
            Cursor.Step += nSteps;
            if (Dt.has_value() && Cursor.SimulationTime.has_value()) {
                *Cursor.SimulationTime += *Dt * static_cast<DevFloat>(nSteps);
            }
            return nSteps > 0;
        }

        [[nodiscard]] auto GetCursorUnsafe() const -> FSimulationCursorV2 override {
            return Cursor;
        }

        [[nodiscard]] auto GetCurrentStateUnsafe() const -> Math::Base::EquationState_constptr override {
            return nullptr;
        }

    public:
        [[nodiscard]] auto SupportsSimulationTime() const -> bool override {
            return Dt.has_value();
        }
    };

    class FBlockingLeaseProbeSessionV2 final : public FSimulationSessionV2 {
        FSimulationCursorV2 Cursor;
        std::optional<DevFloat> Dt;

        mutable std::mutex GateMutex;
        std::condition_variable GateCV;
        bool bInsideStep = false;
        bool bAllowStepFinish = false;

    public:
        explicit FBlockingLeaseProbeSessionV2(std::optional<DevFloat> dt = 0.25)
        : Dt(dt) {
            Cursor.Step = 0;
            Cursor.SimulationTime = dt.has_value() ? std::optional<DevFloat>(0.0) : std::nullopt;
        }

        auto InitializeForCurrentThread() -> void override {
        }

        auto WaitUntilStepEntered(std::chrono::milliseconds timeout = std::chrono::milliseconds(250)) -> bool {
            std::unique_lock lock(GateMutex);
            return GateCV.wait_for(lock, timeout, [this] { return bInsideStep; });
        }

        auto AllowStepToFinish() -> void {
            {
                std::lock_guard lock(GateMutex);
                bAllowStepFinish = true;
            }
            GateCV.notify_all();
        }

    protected:
        auto StepUnsafe(UIntBig nSteps) -> bool override {
            {
                std::lock_guard lock(GateMutex);
                bInsideStep = true;
            }
            GateCV.notify_all();

            {
                std::unique_lock lock(GateMutex);
                GateCV.wait(lock, [this] { return bAllowStepFinish; });
            }

            Cursor.Step += nSteps;
            if (Dt.has_value() && Cursor.SimulationTime.has_value()) {
                *Cursor.SimulationTime += *Dt * static_cast<DevFloat>(nSteps);
            }

            return nSteps > 0;
        }

        [[nodiscard]] auto GetCursorUnsafe() const -> FSimulationCursorV2 override {
            return Cursor;
        }

        [[nodiscard]] auto GetCurrentStateUnsafe() const -> Math::Base::EquationState_constptr override {
            return nullptr;
        }

    public:
        [[nodiscard]] auto SupportsSimulationTime() const -> bool override {
            return Dt.has_value();
        }
    };

    class FSleepingCountingSessionV2 final : public FSimulationSessionV2 {
        FSimulationCursorV2 Cursor;
        std::chrono::milliseconds StepSleep;

    public:
        explicit FSleepingCountingSessionV2(const std::chrono::milliseconds stepSleep = std::chrono::milliseconds(5))
        : StepSleep(stepSleep) {
            Cursor.Step = 0;
            Cursor.SimulationTime = std::nullopt;
        }

        auto InitializeForCurrentThread() -> void override {
        }

    protected:
        auto StepUnsafe(const UIntBig nSteps) -> bool override {
            if (nSteps == 0) return false;
            std::this_thread::sleep_for(StepSleep);
            Cursor.Step += nSteps;
            return true;
        }

        [[nodiscard]] auto GetCursorUnsafe() const -> FSimulationCursorV2 override {
            return Cursor;
        }

        [[nodiscard]] auto GetCurrentStateUnsafe() const -> Math::Base::EquationState_constptr override {
            return nullptr;
        }

    public:
        [[nodiscard]] auto SupportsSimulationTime() const -> bool override {
            return false;
        }
    };

    class FRecordingListenerV2 : public IListenerV2 {
    protected:
        Vector<FRecordedEventV2> Events;
        Str Name;

        auto Record(const FSimulationEventV2 &event) -> void {
            Events.push_back({event.Cursor.Step, event.Reason, event.Cursor.SimulationTime});
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

    class FAbortAfterNSamplesListenerV2 final : public FRecordingListenerV2 {
        size_t MaxSamples;
        size_t SeenSamples = 0;
        FNumericTaskV2 *Task = nullptr;

    public:
        explicit FAbortAfterNSamplesListenerV2(const size_t maxSamples)
        : FRecordingListenerV2("V2 Abort After N Samples Listener")
        , MaxSamples(maxSamples) {
        }

        auto SetTask(FNumericTaskV2 *task) -> void {
            Task = task;
        }

        auto OnSample(const FSimulationEventV2 &event) -> void override {
            FRecordingListenerV2::OnSample(event);
            ++SeenSamples;
            if (Task != nullptr && SeenSamples >= MaxSamples) Task->Abort();
        }
    };

    class FSlowLatestOnlyListenerV2 final : public IListenerV2 {
        mutable std::mutex Mutex;
        Vector<UIntBig> SampleSteps;
        std::chrono::milliseconds Delay;

    public:
        explicit FSlowLatestOnlyListenerV2(std::chrono::milliseconds delay = std::chrono::milliseconds(10))
        : Delay(delay) {
        }

        auto OnSample(const FSimulationEventV2 &event) -> void override {
            std::this_thread::sleep_for(Delay);
            std::lock_guard lock(Mutex);
            SampleSteps.push_back(event.Cursor.Step);
        }

        [[nodiscard]] auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool override {
            (void) finalEvent;
            return true;
        }

        [[nodiscard]] auto GetName() const -> Str override {
            return "Slow LatestOnly Listener";
        }

        [[nodiscard]] auto GetSampleSteps() const -> Vector<UIntBig> {
            std::lock_guard lock(Mutex);
            return SampleSteps;
        }
    };

    class FTestRecipeV2 final : public FSimulationRecipeV2 {
        std::function<TPointer<FSimulationSessionV2>()> SessionBuilder;
        Vector<FSubscriptionV2> Subscriptions;
        FRunLimitsV2 Limits;

    public:
        FTestRecipeV2(std::function<TPointer<FSimulationSessionV2>()> sessionBuilder,
                      Vector<FSubscriptionV2> subscriptions,
                      FRunLimitsV2 limits)
        : SessionBuilder(std::move(sessionBuilder))
        , Subscriptions(std::move(subscriptions))
        , Limits(std::move(limits)) {
        }

        auto BuildSession() -> TPointer<FSimulationSessionV2> override {
            return SessionBuilder();
        }

        auto BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> override {
            return Subscriptions;
        }

        [[nodiscard]] auto GetRunLimits() const -> FRunLimitsV2 override {
            return Limits;
        }
    };

    class FProbeDelegatingRecipeV2 final : public FSimulationRecipeV2 {
        TPointer<FSimulationSessionV2> SessionToReturn;
        Vector<FSubscriptionV2> Subscriptions;
        FRunLimitsV2 Limits;

    public:
        bool bSetupCalled = false;
        size_t BuildSessionCalls = 0;
        size_t BuildSubscriptionsCalls = 0;

        FProbeDelegatingRecipeV2(TPointer<FSimulationSessionV2> sessionToReturn,
                                 Vector<FSubscriptionV2> subscriptions,
                                 FRunLimitsV2 limits)
        : SessionToReturn(std::move(sessionToReturn))
        , Subscriptions(std::move(subscriptions))
        , Limits(std::move(limits)) {
        }

        auto SetupForCurrentThread() -> void override {
            bSetupCalled = true;
        }

        auto BuildSession() -> TPointer<FSimulationSessionV2> override {
            ++BuildSessionCalls;
            return SessionToReturn;
        }

        auto BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> override {
            ++BuildSubscriptionsCalls;
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

    auto SumAbsValues(const RealArray &values) -> DevFloat {
        DevFloat sum = 0.0;
        for (const auto value : values) sum += std::fabs(value);
        return sum;
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
        []() -> TPointer<FSimulationSessionV2> { return New<FCountingSessionV2>(); },
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
        []() -> TPointer<FSimulationSessionV2> { return New<FCountingSessionV2>(); },
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

TEST_CASE("PhaseB V2 - WindowedEveryNStepsTrigger matches windowed contract", "[V2][PhaseB][Trigger]") {
    using namespace Slab::Math::Numerics::V2;

    FWindowedEveryNStepsTriggerV2 trigger(10, 12, 39);
    trigger.Reset(FSimulationCursorV2{.Step = 0});

    const auto next1 = trigger.GetNextDueStepAfter(FSimulationCursorV2{.Step = 0});
    const auto next2 = trigger.GetNextDueStepAfter(FSimulationCursorV2{.Step = 12});
    const auto next3 = trigger.GetNextDueStepAfter(FSimulationCursorV2{.Step = 22});
    const auto next4 = trigger.GetNextDueStepAfter(FSimulationCursorV2{.Step = 32});

    REQUIRE(next1.has_value());
    REQUIRE(next2.has_value());
    REQUIRE(next3.has_value());

    CHECK(*next1 == 12);
    CHECK(*next2 == 22);
    CHECK(*next3 == 32);
    CHECK_FALSE(next4.has_value());

    CHECK(trigger.IsDue(FSimulationCursorV2{.Step = 12}));
    CHECK(trigger.IsDue(FSimulationCursorV2{.Step = 22}));
    CHECK(trigger.IsDue(FSimulationCursorV2{.Step = 32}));
    CHECK_FALSE(trigger.IsDue(FSimulationCursorV2{.Step = 11}));
    CHECK_FALSE(trigger.IsDue(FSimulationCursorV2{.Step = 39}));
    CHECK_FALSE(trigger.IsDue(FSimulationCursorV2{.Step = 40}));
}

TEST_CASE("PhaseB V2 - NumericTask supports final-only subscriptions without trigger", "[V2][PhaseB][Task]") {
    using namespace Slab::Math::Numerics::V2;

    auto finalOnly = New<FRecordingListenerV2>("final-only");
    auto dummy = New<FDummyListenerV2>();

    Vector<FSubscriptionV2> subscriptions = {
        {nullptr, finalOnly, EDeliveryModeV2::Synchronous, false, true},
        {New<FEveryNStepsTriggerV2>(100), dummy, EDeliveryModeV2::Synchronous, false, false}
    };

    FRunLimitsV2 limits;
    limits.Mode = ERunModeV2::FiniteSteps;
    limits.MaxSteps = 7;

    auto recipe = New<FTestRecipeV2>(
        []() -> TPointer<FSimulationSessionV2> { return New<FCountingSessionV2>(); },
        subscriptions,
        limits);

    auto task = New<FNumericTaskV2>(recipe, false, 16);
    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto &events = finalOnly->GetEvents();
    REQUIRE(events.size() == 1);
    CHECK(events[0].Step == 7);
    CHECK(events[0].Reason == EEventReasonV2::Final);
}

TEST_CASE("PhaseB V2 - Windowed trigger integrates with task and console listener", "[V2][PhaseB][Task][Windowed]") {
    using namespace Slab::Math::Numerics::V2;

    auto recorder = New<FRecordingListenerV2>("windowed-recorder");
    auto console = New<FConsoleProgressListenerV2>(15, "console-smoke");

    Vector<FSubscriptionV2> subscriptions = {
        {New<FWindowedEveryNStepsTriggerV2>(4, 3, 12), recorder, EDeliveryModeV2::Synchronous, false, true},
        {New<FEveryNStepsTriggerV2>(5), console, EDeliveryModeV2::Synchronous, true, true}
    };

    FRunLimitsV2 limits;
    limits.Mode = ERunModeV2::FiniteSteps;
    limits.MaxSteps = 15;

    auto recipe = New<FTestRecipeV2>(
        []() -> TPointer<FSimulationSessionV2> { return New<FCountingSessionV2>(); },
        subscriptions,
        limits);

    auto task = New<FNumericTaskV2>(recipe, false, 32);
    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto events = recorder->GetEvents();
    const Vector<UIntBig> expectedSteps = {3, 7, 11, 15};
    const Vector<EEventReasonV2> expectedReasons = {
        EEventReasonV2::Scheduled,
        EEventReasonV2::Scheduled,
        EEventReasonV2::Scheduled,
        EEventReasonV2::Final
    };

    CHECK(EventSteps(events) == expectedSteps);
    CHECK(EventReasons(events) == expectedReasons);
}

TEST_CASE("PhaseC0 V2 - Metropolis Hamiltonian recipe runs through NumericTaskV2", "[V2][PhaseC0][Metropolis][Task]") {
    using namespace Slab;
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Models::KGRtoR::Metropolis::V2;

    RandUtils::SeedUniformReal(59);
    RandUtils::SeedUniformUInt(61);
    RandUtils::SeedGaussianNoise(67);

    constexpr UIntBig maxSteps = 6;
    auto recipe = New<FRtoRHamiltonianMetropolisHastingsRecipeV2>(maxSteps, 1000);
    auto task = New<FNumericTaskV2>(recipe, false, 16);

    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    CHECK(task->GetCursor().Step == maxSteps);

    const auto progress = task->GetProgress01();
    REQUIRE(progress.has_value());
    CHECK(*progress == Catch::Approx(1.0f).margin(1e-6f));

    const auto phiField = recipe->GetPhiField();
    const auto piField = recipe->GetPiField();

    REQUIRE(phiField != nullptr);
    REQUIRE(piField != nullptr);

    const auto phiAbs = SumAbsValues(phiField->getSpace().getHostData(true));
    const auto piAbs = SumAbsValues(piField->getSpace().getHostData(true));

    CAPTURE(phiAbs, piAbs);

    CHECK(phiAbs > 0.0);
    CHECK(piAbs > 0.0);
}

TEST_CASE("PhaseC5 V2 - SPI recipe runs with simulation-time cursor semantics", "[V2][PhaseC5][SPI][Task]") {
    using namespace Slab;
    using namespace Slab::Core;
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Models::StochasticPathIntegrals;
    using namespace Slab::Models::StochasticPathIntegrals::V2;

    auto numericConfig = New<SPINumericConfig>();
    auto iface = numericConfig->GetInterface();

    auto pL = DynamicPointerCast<RealParameter>(iface->GetParameter("length"));
    auto pT = DynamicPointerCast<RealParameter>(iface->GetParameter("time"));
    auto pN = DynamicPointerCast<IntegerParameter>(iface->GetParameter("site_count"));
    auto pDT = DynamicPointerCast<RealParameter>(iface->GetParameter("dT"));
    auto pNT = DynamicPointerCast<IntegerParameter>(iface->GetParameter("stochastic_time_steps"));

    REQUIRE(pL != nullptr);
    REQUIRE(pT != nullptr);
    REQUIRE(pN != nullptr);
    REQUIRE(pDT != nullptr);
    REQUIRE(pNT != nullptr);

    pL->SetValue(1.0);
    pT->SetValue(0.5);
    pN->SetValue(16);
    pDT->SetValue(0.125);
    pNT->SetValue(4);

    auto recipe = New<FSPIRecipeV2>(numericConfig, 2);
    auto task = New<FNumericTaskV2>(recipe, false, 8);

    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto cursor = task->GetCursor();
    CHECK(cursor.Step == 4);
    REQUIRE(cursor.SimulationTime.has_value());
    CHECK(*cursor.SimulationTime == Catch::Approx(0.5).margin(1e-12));

    const auto progress = task->GetProgress01();
    REQUIRE(progress.has_value());
    CHECK(*progress == Catch::Approx(1.0f).margin(1e-6f));

    const auto *session = task->GetSession();
    REQUIRE(session != nullptr);

    const auto state = session->GetCurrentState();
    REQUIRE(state != nullptr);

    auto spiState = std::dynamic_pointer_cast<const SPIState>(state);
    REQUIRE(spiState != nullptr);

    const auto phiAbs = SumAbsValues(spiState->getPhi()->getSpace().getHostData(true));
    CAPTURE(phiAbs);
    CHECK(std::isfinite(phiAbs));
    CHECK(phiAbs > 0.0);
}

TEST_CASE("PhaseR0 V2 - KGRtoR plane waves recipe runs with finite time cursor semantics", "[V2][PhaseR0][KGRtoR][RtoR][Task]") {
    using namespace Slab;
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Models::KGRtoR::PlaneWaves::V2;

    FKGRtoRPlaneWavesConfigV2 config;
    config.N = 64;
    config.L = 10.0;
    config.Dt = 0.01;
    config.Steps = 6;
    config.XCenter = 0.0;
    config.Q = 0.2;
    config.Harmonic = 2;

    auto recipe = New<FKGRtoRPlaneWavesRecipeV2>(config, 1000);
    auto task = New<FNumericTaskV2>(recipe, false, 8);

    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto cursor = task->GetCursor();
    CHECK(cursor.Step == config.Steps);
    REQUIRE(cursor.SimulationTime.has_value());
    CHECK(*cursor.SimulationTime == Catch::Approx(config.Dt * static_cast<DevFloat>(config.Steps)).margin(1e-12));

    const auto progress = task->GetProgress01();
    REQUIRE(progress.has_value());
    CHECK(*progress == Catch::Approx(1.0f).margin(1e-6f));

    const auto *session = task->GetSession();
    REQUIRE(session != nullptr);

    auto lease = session->AcquireReadLease();
    REQUIRE(lease.OwnsLock());
    REQUIRE(lease.GetState() != nullptr);

    auto kgState = std::dynamic_pointer_cast<const Slab::Models::KGRtoR::FEquationState>(lease.GetState());
    REQUIRE(kgState != nullptr);

    const auto &phiData = kgState->getPhi().getSpace().getHostData(true);
    const auto &dPhiDtData = kgState->getDPhiDt().getSpace().getHostData(true);

    const auto phiAbs = SumAbsValues(phiData);
    const auto dPhiDtAbs = SumAbsValues(dPhiDtData);

    CAPTURE(phiAbs, dPhiDtAbs);
    CHECK(std::isfinite(phiAbs));
    CHECK(std::isfinite(dPhiDtAbs));
    CHECK(phiAbs > 0.0);
    CHECK(dPhiDtAbs > 0.0);
}

TEST_CASE("PhaseKG2D0 V2 - KGR2toR baseline recipe runs with finite time cursor semantics",
          "[V2][PhaseKG2D0][KGR2toR][R2toR][Task]") {
    using namespace Slab;
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Models::KGR2toR::Baseline::V2;

    FKGR2toRBaselineConfigV2 config;
    config.N = 96;
    config.L = 12.0;
    config.RDt = 0.1;
    config.Steps = 8;
    config.PulseWidth = 0.35;
    config.PhiAmplitude = 0.0;
    config.DPhiDtAmplitude = 0.8;

    auto recipe = New<FKGR2toRBaselineRecipeV2>(config, 1000);
    auto task = New<FNumericTaskV2>(recipe, false, 16);

    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto cursor = task->GetCursor();
    CHECK(cursor.Step == config.Steps);
    REQUIRE(cursor.SimulationTime.has_value());
    const auto expectedDt = config.RDt * (config.L / static_cast<DevFloat>(config.N));
    CHECK(*cursor.SimulationTime == Catch::Approx(expectedDt * static_cast<DevFloat>(config.Steps)).margin(1e-12));

    const auto progress = task->GetProgress01();
    REQUIRE(progress.has_value());
    CHECK(*progress == Catch::Approx(1.0f).margin(1e-6f));

    const auto *session = task->GetSession();
    REQUIRE(session != nullptr);

    auto lease = session->AcquireReadLease();
    REQUIRE(lease.OwnsLock());
    REQUIRE(lease.GetState() != nullptr);

    auto kgState = std::dynamic_pointer_cast<const Slab::Math::R2toR::EquationState>(lease.GetState());
    REQUIRE(kgState != nullptr);

    const auto &phiData = kgState->getPhi().getSpace().getHostData(true);
    const auto &dPhiDtData = kgState->getDPhiDt().getSpace().getHostData(true);

    const auto phiAbs = SumAbsValues(phiData);
    const auto dPhiDtAbs = SumAbsValues(dPhiDtData);

    CAPTURE(phiAbs, dPhiDtAbs);
    CHECK(std::isfinite(phiAbs));
    CHECK(std::isfinite(dPhiDtAbs));
    CHECK(dPhiDtAbs > 0.0);
}

TEST_CASE("PhaseD V2 - Session try-read lease is best-effort under active writer", "[V2][PhaseD][Session][Lease]") {
    using namespace Slab::Math::Numerics::V2;

    FBlockingLeaseProbeSessionV2 session(0.5);

    std::thread writer([&session] { session.Step(1); });

    REQUIRE(session.WaitUntilStepEntered());

    const auto whileWriting = session.TryAcquireReadLease();
    CHECK_FALSE(whileWriting.has_value());

    session.AllowStepToFinish();
    writer.join();

    const auto afterWrite = session.TryAcquireReadLease();
    REQUIRE(afterWrite.has_value());
    CHECK(afterWrite->OwnsLock());
    CHECK(afterWrite->GetPublishedVersion() == 1);
    CHECK(afterWrite->GetState() == nullptr);
    CHECK(afterWrite->GetCursor().Step == 1);
    REQUIRE(afterWrite->GetCursor().SimulationTime.has_value());
    CHECK(*afterWrite->GetCursor().SimulationTime == Catch::Approx(0.5).margin(1e-12));
}

TEST_CASE("PhaseD V2 - Session blocking read lease captures coherent cursor and version", "[V2][PhaseD][Session][Lease]") {
    using namespace Slab::Math::Numerics::V2;

    FCountingSessionV2 session(0.125);

    CHECK(session.GetPublishedVersion() == 0);
    session.Step(4);
    CHECK(session.GetPublishedVersion() == 1);

    auto lease = session.AcquireReadLease();
    REQUIRE(lease.OwnsLock());
    CHECK(lease.GetPublishedVersion() == 1);
    CHECK(lease.GetState() == nullptr);
    CHECK(lease.GetCursor().Step == 4);
    REQUIRE(lease.GetCursor().SimulationTime.has_value());
    CHECK(*lease.GetCursor().SimulationTime == Catch::Approx(0.5).margin(1e-12));
}

TEST_CASE("PhaseE V2 - Live view exposes lease while active and invalidates on final event", "[V2][PhaseE][LiveData]") {
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Math::LiveData::V2;

    auto liveView = New<FSessionLiveViewV2>();
    auto publisher = New<FSessionLiveViewPublisherListenerV2>(liveView, "test-live-view");
    auto session = New<FCountingSessionV2>(0.25);

    session->Step(5);
    auto sessionLease = session->AcquireReadLease();

    FSimulationEventV2 event;
    event.Cursor = sessionLease.GetCursor();
    event.State = sessionLease.GetState();
    event.Reason = EEventReasonV2::Scheduled;
    event.SessionRef = TPointer<const FSimulationSessionV2>(session);
    event.PublishedVersion = sessionLease.GetPublishedVersion();

    publisher->OnSample(event);

    REQUIRE(liveView->HasBoundSession());

    const auto telemetry = liveView->TryGetTelemetry();
    REQUIRE(telemetry.has_value());
    CHECK(telemetry->LastReason == EEventReasonV2::Scheduled);
    CHECK(telemetry->Cursor.Step == 5);
    REQUIRE(telemetry->Cursor.SimulationTime.has_value());
    CHECK(*telemetry->Cursor.SimulationTime == Catch::Approx(1.25).margin(1e-12));
    CHECK(telemetry->PublishedVersion == 1);
    CHECK_FALSE(telemetry->bRealtimeBestEffort);

    const auto leaseOpt = liveView->AcquireReadLease();
    REQUIRE(leaseOpt.has_value());
    CHECK(leaseOpt->OwnsLock());
    CHECK(leaseOpt->GetCursor().Step == telemetry->Cursor.Step);
    CHECK(leaseOpt->GetPublishedVersion() == telemetry->PublishedVersion);

    event.Reason = EEventReasonV2::Final;
    publisher->OnRunFinished(event);

    CHECK_FALSE(liveView->HasBoundSession());
    CHECK_FALSE(liveView->AcquireReadLease().has_value());

    const auto finalTelemetry = liveView->TryGetTelemetry();
    REQUIRE(finalTelemetry.has_value());
    CHECK(finalTelemetry->LastReason == EEventReasonV2::Final);
    CHECK(finalTelemetry->Cursor.Step == 5);
}

TEST_CASE("PhaseE V2 - SPI recipe can publish a live session view", "[V2][PhaseE][SPI][LiveData]") {
    using namespace Slab;
    using namespace Slab::Core;
    using namespace Slab::Math::LiveData::V2;
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Models::StochasticPathIntegrals;
    using namespace Slab::Models::StochasticPathIntegrals::V2;

    auto numericConfig = New<SPINumericConfig>();
    auto iface = numericConfig->GetInterface();

    auto pL = DynamicPointerCast<RealParameter>(iface->GetParameter("length"));
    auto pT = DynamicPointerCast<RealParameter>(iface->GetParameter("time"));
    auto pN = DynamicPointerCast<IntegerParameter>(iface->GetParameter("site_count"));
    auto pDT = DynamicPointerCast<RealParameter>(iface->GetParameter("dT"));
    auto pNT = DynamicPointerCast<IntegerParameter>(iface->GetParameter("stochastic_time_steps"));

    REQUIRE(pL != nullptr);
    REQUIRE(pT != nullptr);
    REQUIRE(pN != nullptr);
    REQUIRE(pDT != nullptr);
    REQUIRE(pNT != nullptr);

    pL->SetValue(1.0);
    pT->SetValue(0.5);
    pN->SetValue(16);
    pDT->SetValue(0.125);
    pNT->SetValue(4);

    auto liveView = New<FSessionLiveViewV2>();
    auto recipe = New<FSPIRecipeV2>(numericConfig, 2, liveView);
    auto task = New<FNumericTaskV2>(recipe, false, 8);

    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto telemetry = liveView->TryGetTelemetry();
    REQUIRE(telemetry.has_value());
    CHECK(telemetry->LastReason == EEventReasonV2::Final);
    CHECK(telemetry->Cursor.Step == 4);
    CHECK(telemetry->bHasState);
    CHECK(telemetry->PublishedVersion >= 1);
    CHECK_FALSE(liveView->HasBoundSession());

    CHECK_FALSE(liveView->AcquireReadLease().has_value());
}

TEST_CASE("PhaseF V2 - LiveDataHub returns stable named session live views", "[V2][PhaseF][LiveData][Hub]") {
    using namespace Slab::Math::LiveData::V2;

    FLiveDataHubV2 hub;

    auto a1 = hub.GetOrCreateSessionLiveView("spi/main");
    auto a2 = hub.GetOrCreateSessionLiveView("spi/main");
    auto b = hub.GetOrCreateSessionLiveView("spi/aux");

    REQUIRE(a1 != nullptr);
    REQUIRE(a2 != nullptr);
    REQUIRE(b != nullptr);

    CHECK(a1 == a2);
    CHECK(a1 != b);

    CHECK(hub.FindSessionLiveView("spi/main") == a1);
    CHECK(hub.FindSessionLiveView("missing") == nullptr);

    const auto topics = hub.ListSessionLiveViewTopics();
    CHECK(std::find(topics.begin(), topics.end(), "spi/main") != topics.end());
    CHECK(std::find(topics.begin(), topics.end(), "spi/aux") != topics.end());
}

TEST_CASE("PhaseF V2 - SessionLiveView facade remains compatible on top of generic topics", "[V2][PhaseF][LiveData]") {
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Math::LiveData::V2;

    auto sessionTopic = New<FSessionViewTopicV2>("topic/session");
    auto telemetryTopic = New<FSessionTelemetryTopicV2>("topic/telemetry");
    auto statusTopic = New<FSessionStatusTopicV2>("topic/status");
    auto liveView = New<FSessionLiveViewV2>(sessionTopic, telemetryTopic, statusTopic);
    auto session = New<FCountingSessionV2>(0.5);

    session->Step(2);
    auto lease = session->AcquireReadLease();

    FSimulationEventV2 event;
    event.Cursor = lease.GetCursor();
    event.State = lease.GetState();
    event.Reason = EEventReasonV2::Scheduled;
    event.SessionRef = TPointer<const FSimulationSessionV2>(session);
    event.PublishedVersion = lease.GetPublishedVersion();

    liveView->PublishEvent(event);

    REQUIRE(liveView->HasBoundSession());

    const auto telemetry = telemetryTopic->TryGetTelemetry();
    REQUIRE(telemetry.has_value());
    CHECK(telemetry->Cursor.Step == 2);
    CHECK(telemetry->PublishedVersion == 1);

    const auto status = statusTopic->TryGetStatus();
    REQUIRE(status.has_value());
    CHECK(status->RunState == ESessionRunStateV2::Running);
    CHECK(status->bHasBoundSession);
    CHECK(status->PublishedVersion == 1);

    auto liveLease = sessionTopic->AcquireReadLease();
    REQUIRE(liveLease.has_value());
    CHECK(liveLease->GetCursor().Step == 2);

    event.Reason = EEventReasonV2::Final;
    liveView->PublishEvent(event);
    CHECK_FALSE(liveView->HasBoundSession());

    const auto finalStatus = liveView->TryGetStatus();
    REQUIRE(finalStatus.has_value());
    CHECK(finalStatus->RunState == ESessionRunStateV2::Finished);
    CHECK(finalStatus->bTerminal);
    CHECK_FALSE(finalStatus->bHasBoundSession);
}

TEST_CASE("PhaseH V2 - LiveControl topic keeps latest typed sample and version", "[V2][PhaseH][LiveControl][Topic]") {
    using namespace Slab::Math::LiveControl::V2;

    FControlTopicV2 topic("labv2/input/mouse/world-pos");
    CHECK(topic.GetName() == "labv2/input/mouse/world-pos");
    CHECK_FALSE(topic.HasSample());
    CHECK_FALSE(topic.TryGetLatestSample().has_value());

    FControlSampleV2 sample;
    sample.Value = Slab::Math::Real2D{1.5, -0.25};
    sample.Semantic = EControlSemanticV2::Level;
    sample.Timestamp.Domain = EControlTimeDomainV2::WallClockTime;
    sample.Timestamp.WallClockSeconds = 0.25;

    topic.Publish(sample);

    REQUIRE(topic.HasSample());
    const auto latest = topic.TryGetLatestSample();
    REQUIRE(latest.has_value());
    CHECK(latest->PublishedVersion == 1);
    CHECK(latest->Semantic == EControlSemanticV2::Level);
    CHECK(latest->Timestamp.Domain == EControlTimeDomainV2::WallClockTime);
    REQUIRE(latest->Timestamp.WallClockSeconds.has_value());
    CHECK(*latest->Timestamp.WallClockSeconds == Catch::Approx(0.25).margin(1e-12));

    const auto point = topic.TryGetLatestPoint2D();
    REQUIRE(point.has_value());
    CHECK(point->x == Catch::Approx(1.5).margin(1e-12));
    CHECK(point->y == Catch::Approx(-0.25).margin(1e-12));
    CHECK_FALSE(topic.TryGetLatestBool().has_value());

    FControlSampleV2 event;
    event.Value = true;
    event.Semantic = EControlSemanticV2::Event;
    event.Timestamp.Domain = EControlTimeDomainV2::StepIndex;
    event.Timestamp.Step = 42;
    topic.Publish(event);

    const auto latestEvent = topic.TryGetLatestSample();
    REQUIRE(latestEvent.has_value());
    CHECK(latestEvent->PublishedVersion == 2);
    CHECK(latestEvent->Semantic == EControlSemanticV2::Event);
    CHECK(latestEvent->Timestamp.Domain == EControlTimeDomainV2::StepIndex);
    REQUIRE(latestEvent->Timestamp.Step.has_value());
    CHECK(*latestEvent->Timestamp.Step == 42);
    REQUIRE(topic.TryGetLatestBool().has_value());
    CHECK(*topic.TryGetLatestBool());
    CHECK_FALSE(topic.TryGetLatestPoint2D().has_value());
}

TEST_CASE("PhaseH V2 - LiveControlHub returns stable named topics", "[V2][PhaseH][LiveControl][Hub]") {
    using namespace Slab::Math::LiveControl::V2;

    FLiveControlHubV2 hub;

    auto a1 = hub.GetOrCreateTopic("labv2/control/a");
    auto a2 = hub.GetOrCreateTopic("labv2/control/a");
    auto b = hub.GetOrCreateTopic("labv2/control/b");

    REQUIRE(a1 != nullptr);
    REQUIRE(a2 != nullptr);
    REQUIRE(b != nullptr);

    CHECK(a1 == a2);
    CHECK(a1 != b);

    CHECK(hub.FindTopic("labv2/control/a") == a1);
    CHECK(hub.FindTopic("missing") == nullptr);

    const auto names = hub.ListTopics();
    CHECK(std::find(names.begin(), names.end(), "labv2/control/a") != names.end());
    CHECK(std::find(names.begin(), names.end(), "labv2/control/b") != names.end());
}

TEST_CASE("PhaseG V2 - OutputScheduler detects wall-clock trigger crossings", "[V2][PhaseG][Scheduler][WallClock]") {
    using namespace Slab::Math::Numerics::V2;

    auto listener = New<FRecordingListenerV2>("wall-clock");
    Vector<FSubscriptionV2> subscriptions = {
        {New<FEveryWallClockTriggerV2>(0.1), listener, EDeliveryModeV2::Synchronous, false, false}
    };

    FOutputSchedulerV2 scheduler;
    scheduler.Reset(subscriptions, FSimulationCursorV2{.Step = 0, .WallClockSeconds = 0.0});

    CHECK(scheduler.CollectDueSubscriptionsBetween(
        subscriptions,
        FSimulationCursorV2{.Step = 1, .WallClockSeconds = 0.05},
        FSimulationCursorV2{.Step = 2, .WallClockSeconds = 0.09}).empty());

    const auto due = scheduler.CollectDueSubscriptionsBetween(
        subscriptions,
        FSimulationCursorV2{.Step = 2, .WallClockSeconds = 0.09},
        FSimulationCursorV2{.Step = 3, .WallClockSeconds = 0.12});
    REQUIRE(due.size() == 1);
    CHECK(due[0] == 0);
}

TEST_CASE("PhaseG V2 - LatestOnly delivery coalesces pending samples and flushes before final", "[V2][PhaseG][Dispatcher][LatestOnly]") {
    using namespace Slab::Math::Numerics::V2;

    auto listener = New<FSlowLatestOnlyListenerV2>(std::chrono::milliseconds(15));
    Vector<FSubscriptionV2> subscriptions = {
        {New<FEveryNStepsTriggerV2>(1), listener, EDeliveryModeV2::LatestOnly, false, true}
    };

    FOutputDispatcherV2 dispatcher;
    const Vector<size_t> dueIndices = {0};

    for (UIntBig step = 1; step <= 20; ++step) {
        FSimulationEventV2 sample;
        sample.Cursor.Step = step;
        sample.Reason = EEventReasonV2::Scheduled;
        dispatcher.DispatchScheduled(subscriptions, dueIndices, sample);
    }

    FSimulationEventV2 finalEvent;
    finalEvent.Cursor.Step = 20;
    finalEvent.Reason = EEventReasonV2::Final;
    CHECK(dispatcher.DispatchRunFinished(subscriptions, finalEvent));

    const auto steps = listener->GetSampleSteps();
    REQUIRE_FALSE(steps.empty());
    CHECK(steps.back() == 20);
    CHECK(steps.size() < 20);
}

TEST_CASE("PhaseG V2 - NumericTask can drive wall-clock triggers in open-ended runs", "[V2][PhaseG][Task][WallClock]") {
    using namespace Slab::Math::Numerics::V2;

    auto listener = New<FAbortAfterNSamplesListenerV2>(3);
    Vector<FSubscriptionV2> subscriptions = {
        {New<FEveryWallClockTriggerV2>(0.01), listener, EDeliveryModeV2::Synchronous, false, true}
    };

    FRunLimitsV2 limits;
    limits.Mode = ERunModeV2::OpenEnded;

    auto recipe = New<FTestRecipeV2>(
        []() -> TPointer<FSimulationSessionV2> { return New<FSleepingCountingSessionV2>(std::chrono::milliseconds(5)); },
        subscriptions,
        limits);

    auto task = New<FNumericTaskV2>(recipe, false, 1);
    listener->SetTask(task.get());

    REQUIRE(RunTaskAndWait(*task) == Core::TaskAborted);

    const auto events = listener->GetEvents();
    REQUIRE_FALSE(events.empty());
    CHECK(events.back().Reason == EEventReasonV2::AbortFinal);

    size_t scheduledCount = 0;
    for (const auto &event : events) if (event.Reason == EEventReasonV2::Scheduled) ++scheduledCount;
    CHECK(scheduledCount >= 3);
}

TEST_CASE("PhaseH V2 - CursorHistoryListener records scheduled and terminal cursors", "[V2][PhaseH][Listener][History]") {
    using namespace Slab::Math::Numerics::V2;

    auto history = New<FCursorHistoryListenerV2>();
    Vector<FSubscriptionV2> subscriptions = {
        {New<FEveryNStepsTriggerV2>(3), history, EDeliveryModeV2::Synchronous, true, true}
    };

    FRunLimitsV2 limits;
    limits.Mode = ERunModeV2::FiniteSteps;
    limits.MaxSteps = 10;

    auto recipe = New<FTestRecipeV2>(
        []() -> TPointer<FSimulationSessionV2> { return New<FCountingSessionV2>(0.25); },
        subscriptions,
        limits);

    auto task = New<FNumericTaskV2>(recipe, false, 16);
    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto &samples = history->GetSamples();
    REQUIRE(samples.size() == 5);
    CHECK(samples.front().Reason == EEventReasonV2::Initial);
    CHECK(samples.front().Cursor.Step == 0);
    CHECK(samples[1].Cursor.Step == 3);
    CHECK(samples[2].Cursor.Step == 6);
    CHECK(samples[3].Cursor.Step == 9);
    CHECK(samples.back().Reason == EEventReasonV2::Final);
    CHECK(samples.back().Cursor.Step == 10);
}

TEST_CASE("PhaseH V2 - StateSnapshotListener captures copied final SPI state", "[V2][PhaseH][Listener][Snapshot][SPI]") {
    using namespace Slab;
    using namespace Slab::Core;
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Models::StochasticPathIntegrals;
    using namespace Slab::Models::StochasticPathIntegrals::V2;

    auto numericConfig = New<SPINumericConfig>();
    auto iface = numericConfig->GetInterface();
    DynamicPointerCast<RealParameter>(iface->GetParameter("length"))->SetValue(1.0);
    DynamicPointerCast<RealParameter>(iface->GetParameter("time"))->SetValue(0.5);
    DynamicPointerCast<IntegerParameter>(iface->GetParameter("site_count"))->SetValue(16);
    DynamicPointerCast<RealParameter>(iface->GetParameter("dT"))->SetValue(0.125);
    DynamicPointerCast<IntegerParameter>(iface->GetParameter("stochastic_time_steps"))->SetValue(4);

    auto snapshot = New<FStateSnapshotListenerV2>("SPI Snapshot");
    auto recipe = New<FSPIRecipeV2>(numericConfig, 1000);

    // Add snapshot listener manually on top of recipe defaults by wrapping recipe outputs into test recipe.
    auto baseSubs = recipe->BuildDefaultSubscriptions();
    baseSubs.push_back({nullptr, snapshot, EDeliveryModeV2::Synchronous, false, true});

    auto wrapperRecipe = New<FTestRecipeV2>(
        [recipe]() -> TPointer<FSimulationSessionV2> { return recipe->BuildSession(); },
        baseSubs,
        recipe->GetRunLimits());

    auto task = New<FNumericTaskV2>(wrapperRecipe, false, 8);
    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    REQUIRE(snapshot->HasSnapshot());
    CHECK(snapshot->GetSnapshotReason() == EEventReasonV2::Final);
    CHECK(snapshot->GetSnapshotCursor().Step == 4);

    const auto *session = task->GetSession();
    REQUIRE(session != nullptr);
    auto currentState = std::dynamic_pointer_cast<const SPIState>(session->GetCurrentState());
    auto snapState = std::dynamic_pointer_cast<const SPIState>(snapshot->GetSnapshot());
    REQUIRE(currentState != nullptr);
    REQUIRE(snapState != nullptr);
    CHECK(currentState.get() != snapState.get());

    const auto currentAbs = SumAbsValues(currentState->getPhi()->getSpace().getHostData(true));
    const auto snapAbs = SumAbsValues(snapState->getPhi()->getSpace().getHostData(true));
    CHECK(snapAbs == Catch::Approx(currentAbs).epsilon(1e-9));
}

TEST_CASE("PhaseH V2 - ScalarTimeDFTListener computes DFT magnitudes from sampled series", "[V2][PhaseH][Listener][DFT]") {
    using namespace Slab::Math::Numerics::V2;

    constexpr auto w = DevFloat(6.0);
    auto dft = New<FScalarTimeDFTListenerV2>(
        [w](const FSimulationEventV2 &event) -> std::optional<DevFloat> {
            if (!event.Cursor.SimulationTime.has_value()) return std::nullopt;
            return std::sin(w * *event.Cursor.SimulationTime);
        });

    for (int i = 0; i < 128; ++i) {
        FSimulationEventV2 event;
        event.Reason = EEventReasonV2::Scheduled;
        event.Cursor.Step = static_cast<UIntBig>(i);
        event.Cursor.SimulationTime = DevFloat(i) * DevFloat(0.01);
        dft->OnSample(event);
    }

    FSimulationEventV2 finalEvent;
    finalEvent.Reason = EEventReasonV2::Final;
    finalEvent.Cursor.Step = 128;
    finalEvent.Cursor.SimulationTime = DevFloat(1.28);
    REQUIRE(dft->OnRunFinished(finalEvent));

    CHECK(dft->GetSampleCount() == 128);
    REQUIRE(dft->GetDFTMagnitudes() != nullptr);
}

TEST_CASE("PhaseI V2 - AppendedSubscriptionsRecipe delegates and preserves run limits", "[V2][PhaseI][Recipe][Wrapper]") {
    using namespace Slab::Math::Numerics::V2;

    auto session = New<FCountingSessionV2>(0.25);
    auto baseListener = New<FRecordingListenerV2>("base");
    auto extraListener = New<FRecordingListenerV2>("extra");

    Vector<FSubscriptionV2> baseSubscriptions = {{
        New<FEveryNStepsTriggerV2>(3),
        baseListener,
        EDeliveryModeV2::Synchronous,
        true,
        true
    }};
    Vector<FSubscriptionV2> extraSubscriptions = {{
        New<FEveryNStepsTriggerV2>(5),
        extraListener,
        EDeliveryModeV2::LatestOnly,
        false,
        true
    }};

    FRunLimitsV2 limits;
    limits.Mode = ERunModeV2::FiniteSteps;
    limits.MaxSteps = 42;

    auto inner = New<FProbeDelegatingRecipeV2>(session, baseSubscriptions, limits);
    FAppendedSubscriptionsRecipeV2 wrapped(inner, extraSubscriptions);

    wrapped.SetupForCurrentThread();
    CHECK(inner->bSetupCalled);

    const auto builtSession = wrapped.BuildSession();
    CHECK(inner->BuildSessionCalls == 1);
    CHECK(builtSession == session);

    const auto merged = wrapped.BuildDefaultSubscriptions();
    CHECK(inner->BuildSubscriptionsCalls == 1);
    REQUIRE(merged.size() == 2);
    CHECK(merged[0].Listener == baseListener);
    CHECK(merged[1].Listener == extraListener);
    CHECK(merged[1].DeliveryMode == EDeliveryModeV2::LatestOnly);
    CHECK_FALSE(merged[1].bWantsInitialEvent);
    CHECK(merged[1].bWantsFinalEvent);

    const auto wrappedLimits = wrapped.GetRunLimits();
    CHECK(wrappedLimits.Mode == ERunModeV2::FiniteSteps);
    REQUIRE(wrappedLimits.MaxSteps.has_value());
    CHECK(*wrappedLimits.MaxSteps == 42);
}

TEST_CASE("PhaseI V2 - AppendedSubscriptionsRecipe integrates extra listeners in task flow", "[V2][PhaseI][Recipe][Task]") {
    using namespace Slab::Math::Numerics::V2;

    auto baseListener = New<FRecordingListenerV2>("base");
    auto extraListener = New<FRecordingListenerV2>("extra");

    FRunLimitsV2 limits;
    limits.Mode = ERunModeV2::FiniteSteps;
    limits.MaxSteps = 6;

    auto inner = New<FTestRecipeV2>(
        [] { return New<FCountingSessionV2>(0.5); },
        Vector<FSubscriptionV2>{{
            New<FEveryNStepsTriggerV2>(3),
            baseListener,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }},
        limits);

    auto wrappedRecipe = New<FAppendedSubscriptionsRecipeV2>(
        inner,
        Vector<FSubscriptionV2>{{
            New<FEveryNStepsTriggerV2>(2),
            extraListener,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }});

    FNumericTaskV2 task(wrappedRecipe, false, 16);
    const auto status = RunTaskAndWait(task);

    CHECK(status == Core::TaskSuccess);
    CHECK(EventSteps(baseListener->GetEvents()) == Vector<UIntBig>{0, 3, 6, 6});
    CHECK(EventSteps(extraListener->GetEvents()) == Vector<UIntBig>{0, 2, 4, 6, 6});
    CHECK(EventReasons(extraListener->GetEvents()).front() == EEventReasonV2::Initial);
    CHECK(EventReasons(extraListener->GetEvents()).back() == EEventReasonV2::Final);
}
