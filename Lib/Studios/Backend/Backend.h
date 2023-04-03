#ifndef BACKEND_H
#define BACKEND_H

#include "Program.h"

class Backend
{
protected:

    static Backend *myInstance;
    explicit Backend(Backend *instance, String name);
    virtual ~Backend();

public:
    static auto GetInstance() -> Backend *;

    static void Destroy();

    virtual void run(Program *integrator) = 0;

    const String backendName;
};

#endif // BACKEND_H
