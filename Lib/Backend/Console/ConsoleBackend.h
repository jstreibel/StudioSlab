#ifndef CONSOLEBACKEND_H
#define CONSOLEBACKEND_H

#include "Backend/Backend.h"

class ConsoleBackend : public Backend
{
    ConsoleBackend();
public:
    static ConsoleBackend *getSingleton();

    // Backend interface
public:
    virtual void run(Program *integrator);

};

#endif // CONSOLEBACKEND_H
