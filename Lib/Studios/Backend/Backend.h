#ifndef BACKEND_H
#define BACKEND_H

#include "Program.h"

class Backend
{
protected:

    static Backend *myInstance;
    explicit Backend(Backend *instance);
    virtual ~Backend();

protected:

public:
    static auto GetInstance() -> Backend *;

    static void Destroy();

    virtual void run(Program *integrator) = 0;
};

#endif // BACKEND_H
