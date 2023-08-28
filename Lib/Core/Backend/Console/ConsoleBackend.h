#ifndef CONSOLEBACKEND_H
#define CONSOLEBACKEND_H

#include "Core/Backend/Backend.h"

class ConsoleBackend : public Backend
{
    friend DerivableSingleton;
protected:
    ConsoleBackend();

public:
    virtual void run(Program *integrator);
};

#endif // CONSOLEBACKEND_H
