#include "ConsoleBackend.h"

ConsoleBackend::ConsoleBackend() : Backend("Console backend")
{
}

void ConsoleBackend::run(Program *integrator) {
    integrator->cycle(Program::CycleOptions::cycleCycleUntilFinished);
}

void ConsoleBackend::finish() {

}
