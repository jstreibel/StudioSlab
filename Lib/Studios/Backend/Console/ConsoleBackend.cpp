#include "ConsoleBackend.h"

ConsoleBackend::ConsoleBackend() : Backend(this) {
    /*
    if(Parse(argc, argv, p) == ERROR_IN_COMMAND_LINE){
        std::cout << "Error in cmd line." << std::endl;
        exit(0);
    }

    if(p.maxTime <= 0)
        throw "Console backend requires maximum time set.";
        */
}

ConsoleBackend *ConsoleBackend::getSingleton()
{
    if(Backend::myInstance == nullptr) return new ConsoleBackend();

    return dynamic_cast<ConsoleBackend*>(Backend::GetInstance());
}

void ConsoleBackend::run(Program *integrator) {
    integrator->runFullIntegration();
}
