#include "TaskServiceV2.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/SlabCore.h"
#include "StudioSlab.h"
#include "Utils/Exception.h"

#include <algorithm>

namespace Slab::Core::Composition::V2 {

    auto FLegacyTaskServiceV2::TryGetLoadedTaskManager() const -> TPointer<Core::FTaskManager> {
        if (!Slab::IsStarted()) return nullptr;
        if (!Core::FBackendManager::IsModuleLoaded("TaskManager")) return nullptr;
        return Core::GetModule<Core::FTaskManager>("TaskManager");
    }

    auto FLegacyTaskServiceV2::GetOrLoadTaskManager() const -> TPointer<Core::FTaskManager> {
        const auto taskManager = Core::GetModule<Core::FTaskManager>("TaskManager");
        if (taskManager == nullptr) throw Exception("TaskManager module not available.");
        return taskManager;
    }

    auto FLegacyTaskServiceV2::AddTask(const TPointer<Core::FTask> &task) -> void {
        if (task == nullptr) throw Exception("TaskService V2 cannot add a null task.");
        GetOrLoadTaskManager()->AddTask(task);
    }

    auto FLegacyTaskServiceV2::HasRunningTasks() const -> bool {
        const auto taskManager = TryGetLoadedTaskManager();
        if (taskManager == nullptr) return false;
        return taskManager->HasRunningTasks();
    }

    auto FLegacyTaskServiceV2::GetTaskCount() const -> std::size_t {
        return GetTaskSnapshots().size();
    }

    auto FLegacyTaskServiceV2::GetRunningTaskCount() const -> std::size_t {
        const auto snapshots = GetTaskSnapshots();
        return static_cast<std::size_t>(std::count_if(
            snapshots.begin(),
            snapshots.end(),
            [](const auto &snapshot) {
                return snapshot.bRunning;
            }));
    }

    auto FLegacyTaskServiceV2::GetTaskSnapshots() const -> Vector<FTaskSnapshotV2> {
        const auto taskManager = TryGetLoadedTaskManager();
        if (taskManager == nullptr) return {};

        Vector<FTaskSnapshotV2> snapshots;
        for (const auto &job : taskManager->GetAllJobs()) {
            FTaskSnapshotV2 snapshot;
            snapshot.TaskIdentity = job.Task.get();
            if (job.Task != nullptr) {
                snapshot.Name = job.Task->GetName();
                snapshot.Status = job.Task->GetStatus();
                snapshot.bRunning = job.Task->IsTaskRunning();
            }
            snapshots.push_back(std::move(snapshot));
        }

        return snapshots;
    }

    auto FLegacyTaskServiceV2::ClearTask(const void *taskIdentity) -> bool {
        if (taskIdentity == nullptr) return false;

        const auto taskManager = TryGetLoadedTaskManager();
        if (taskManager == nullptr) return false;

        for (const auto &job : taskManager->GetAllJobs()) {
            if (job.Task.get() != taskIdentity) continue;
            return taskManager->ClearJob(job);
        }

        return false;
    }

} // namespace Slab::Core::Composition::V2
