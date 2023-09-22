#ifndef CONSOLEBACKEND_H
#define CONSOLEBACKEND_H

#include "Core/Backend/Backend.h"

class ConsoleBackend : public Core::Backend
{
public:
    ConsoleBackend();
    void run(Program *integrator) override;
};

#endif // CONSOLEBACKEND_H
