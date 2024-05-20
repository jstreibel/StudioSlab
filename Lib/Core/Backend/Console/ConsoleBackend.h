#ifndef CONSOLEBACKEND_H
#define CONSOLEBACKEND_H

#include "Core/Backend/Backend.h"

class ConsoleBackend : public Core::Backend
{
public:
    ConsoleBackend();
    void run(Program *integrator) override;

    void terminate() override;
};

#endif // CONSOLEBACKEND_H
