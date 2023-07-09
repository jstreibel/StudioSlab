#include "ConsoleBackend.h"

ConsoleBackend::ConsoleBackend() : Backend("Console backend") {
    /*
    if(Parse(argc, argv, p) == ERROR_IN_COMMAND_LINE){
        std::cout << "Error in cmd line." << std::endl;
        exit(0);
    }

    if(p.maxTime <= 0)
        throw "Console backend requires maximum time set.";
        */
}

void ConsoleBackend::run(Program *integrator) {
    integrator->cycle(Program::CycleOptions::cycleCycleUntilFinished);
}
