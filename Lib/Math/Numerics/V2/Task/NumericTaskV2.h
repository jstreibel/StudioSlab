#ifndef STUDIOSLAB_NUMERIC_TASK_V2_H
#define STUDIOSLAB_NUMERIC_TASK_V2_H

#include "Math/Numerics/V2/Listeners/OutputDispatcherV2.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/OutputSchedulerV2.h"

#include "Core/Backend/Modules/TaskManager/Task.h"

#include <atomic>
#include <optional>

namespace Slab::Math::Numerics::V2 {

    class FNumericTaskV2 : public Core::FTask {
        TPointer<FSimulationRecipeV2> Recipe;
        TUnique<FSimulationSessionV2> Session;

        Vector<FSubscriptionV2> Subscriptions;
        FRunLimitsV2 RunLimits;

        FOutputSchedulerV2 Scheduler;
        FOutputDispatcherV2 Dispatcher;

        std::atomic<bool> bAbortFlag = false;

        size_t MaxBatchSteps = 2048;
        bool bInitialized = false;

        [[nodiscard]] auto HasReachedFiniteLimit(const FSimulationCursorV2 &cursor) const -> bool;
        [[nodiscard]] auto ComputeBatchSteps(const FSimulationCursorV2 &cursor,
                                            const std::optional<UIntBig> &nextDueStep) const -> UIntBig;
        [[nodiscard]] auto BuildEvent(EEventReasonV2 reason, bool bRealtimeBestEffort = false) const
                -> FSimulationEventV2;

        auto EmitInitialEventIfRequested() -> void;
        auto EmitFinalEvent(EEventReasonV2 reason) -> bool;

    protected:
        auto Run() -> Core::ETaskStatus override;

    public:
        explicit FNumericTaskV2(const TPointer<FSimulationRecipeV2> &recipe,
                                bool preInit = true,
                                size_t maxBatchSteps = 2048);

        auto Init() -> void;
        [[nodiscard]] auto IsInitialized() const -> bool;

        auto Abort() -> void override;

        [[nodiscard]] auto GetCursor() const -> FSimulationCursorV2;
        [[nodiscard]] auto GetProgress01() const -> std::optional<float>;
        [[nodiscard]] auto GetSession() const -> const FSimulationSessionV2 *;
    };

    DefinePointers(FNumericTaskV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_NUMERIC_TASK_V2_H
