#ifndef STUDIOSLAB_TASK_SERVICE_V2_H
#define STUDIOSLAB_TASK_SERVICE_V2_H

#include "Core/Backend/Modules/TaskManager/Task.h"
#include "Utils/Arrays.h"
#include "Utils/Pointer.h"
#include "Utils/String.h"

#include <cstddef>

namespace Slab::Core {
    class FTaskManager;
}

namespace Slab::Core::Composition::V2 {

    struct FTaskSnapshotV2 {
        const void *TaskIdentity = nullptr;
        Str Name;
        Core::ETaskStatus Status = Core::TaskNotInitialized;
        bool bRunning = false;
    };

    class ITaskServiceV2 {
    public:
        virtual ~ITaskServiceV2() = default;

        virtual auto AddTask(const TPointer<Core::FTask> &task) -> void = 0;

        [[nodiscard]] virtual auto HasRunningTasks() const -> bool = 0;
        [[nodiscard]] virtual auto GetTaskCount() const -> std::size_t = 0;
        [[nodiscard]] virtual auto GetRunningTaskCount() const -> std::size_t = 0;
        [[nodiscard]] virtual auto GetTaskSnapshots() const -> Vector<FTaskSnapshotV2> = 0;

        virtual auto ClearTask(const void *taskIdentity) -> bool = 0;
    };

    DefinePointers(ITaskServiceV2)

    class FLegacyTaskServiceV2 final : public ITaskServiceV2 {
    public:
        auto AddTask(const TPointer<Core::FTask> &task) -> void override;

        [[nodiscard]] auto HasRunningTasks() const -> bool override;
        [[nodiscard]] auto GetTaskCount() const -> std::size_t override;
        [[nodiscard]] auto GetRunningTaskCount() const -> std::size_t override;
        [[nodiscard]] auto GetTaskSnapshots() const -> Vector<FTaskSnapshotV2> override;

        auto ClearTask(const void *taskIdentity) -> bool override;

    private:
        [[nodiscard]] auto TryGetLoadedTaskManager() const -> TPointer<Core::FTaskManager>;
        [[nodiscard]] auto GetOrLoadTaskManager() const -> TPointer<Core::FTaskManager>;
    };

    DefinePointers(FLegacyTaskServiceV2)

} // namespace Slab::Core::Composition::V2

#endif // STUDIOSLAB_TASK_SERVICE_V2_H
