#include "ConsoleBackend.h"


namespace Slab::Core {


    ConsoleBackend::ConsoleBackend() : Backend("Console backend") {
    }

    void ConsoleBackend::run(Task *integrator) {
        integrator->cycle(Task::CycleOptions::cycleCycleUntilFinished);
    }

    void ConsoleBackend::terminate() {

    }

}