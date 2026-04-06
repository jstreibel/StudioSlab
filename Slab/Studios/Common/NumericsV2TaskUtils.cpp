#include "NumericsV2TaskUtils.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace Slab::Studios::Common {

    auto RunTaskAndWait(Core::FTask &task) -> Core::ETaskStatus {
        std::thread worker([&task] { task.Start(); });

        while (true) {
            const auto status = task.GetStatus();
            if (status != Core::TaskNotInitialized && status != Core::TaskRunning) {
                task.Release();
                if (worker.joinable()) worker.join();
                return status;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    auto RunTaskWithVisualHost(const FVisualHostContext &host,
                               Core::FTask &task) -> Core::ETaskStatus {
        std::thread worker([&task] { task.Start(); });

        const auto cleanupTaskThread = [&task, &worker]() {
            task.Abort();
            task.Release();
            if (worker.joinable()) worker.join();
        };

        try {
            RunVisualHost(host);
        } catch (...) {
            cleanupTaskThread();
            throw;
        }

        cleanupTaskThread();
        return task.GetStatus();
    }

    auto ExitCodeFromTaskStatus(const Core::ETaskStatus status) -> int {
        switch (status) {
        case Core::TaskSuccess:
            return 0;
        case Core::TaskAborted:
            return 130;
        case Core::TaskError:
        case Core::TaskNotInitialized:
        case Core::TaskRunning:
            return 2;
        }

        return 2;
    }

    auto PrintNumericTaskSummary(const Math::Numerics::V2::FNumericTaskV2 &task) -> void {
        const auto cursor = task.GetCursor();
        std::cout << '\n' << "Finished at step=" << cursor.Step;
        if (cursor.SimulationTime.has_value()) {
            std::cout << " t=" << *cursor.SimulationTime;
        }
        std::cout << '\n';
    }

} // namespace Slab::Studios::Common
