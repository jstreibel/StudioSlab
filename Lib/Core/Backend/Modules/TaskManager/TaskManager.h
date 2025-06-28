//
// Created by joao on 23/05/24.
//

#ifndef STUDIOSLAB_TASKMANAGER_H
#define STUDIOSLAB_TASKMANAGER_H

#include <thread>
#include "../Module.h"

#include "Task.h"

namespace Slab::Core {

        class MTaskManager final : public SlabModule {
            typedef Pair<FTask_ptr, Pointer<std::thread>> Job;
            Vector<Job> Jobs;

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

            /**
             * Signals all finished jobs to end their threads in a safe manner.
             */
            void PruneThreads();

            static void Abort(const Job& Job);

            /**
             * Add a task to the manager and start it immediately in a new thread.
             */
            Job AddTask(const Pointer<FTask>& Task);

            void AbortAllTasks() const;


        };

} // Slab::Core

#endif //STUDIOSLAB_TASKMANAGER_H
