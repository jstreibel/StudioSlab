//
// Created by joao on 23/05/24.
//

#ifndef STUDIOSLAB_TASKMANAGER_H
#define STUDIOSLAB_TASKMANAGER_H

#include <thread>
#include "../Module.h"

#include "Task.h"
#include "Utils/List.h"

namespace Slab::Core {

        class MTaskManager final : public SlabModule {
            using FTaskPointer = TPointer<FTask>;
            struct FJob
            {
                FTaskPointer Task;
                TPointer<std::thread> JobThread;

                bool operator==(const FJob& RHS) const { return Task == RHS.Task && JobThread == RHS.JobThread; }
            };
            TList<FJob> Jobs;

            std::mutex AddJobMutex;

            enum EDestructorPolicy {
                AbortAll,
                WaitAll
            } destructorPolicy;

        public:
            explicit MTaskManager(EDestructorPolicy Policy=AbortAll);
            // Destructor to join all threads
            ~MTaskManager() override;

            [[nodiscard]] auto HasRunningTasks() const -> bool;
            [[nodiscard]] auto GetNumberOfRunningTasks() const -> size_t;

            TList<FJob> GetAllJobs();

            bool ClearJob(const FJob&);

            static void Abort(const FJob& Job);

            /**
             * Add a task to the manager and start it immediately in a new thread.
             */
            FJob AddTask(const TPointer<FTask>& Task);

            void AbortAllTasks() const;


        };

} // Slab::Core

#endif //STUDIOSLAB_TASKMANAGER_H
