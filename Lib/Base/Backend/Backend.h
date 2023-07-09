#ifndef BACKEND_H
#define BACKEND_H

#include "Program.h"
#include "Common/Singleton.h"
#include "Events/MouseState.h"

class Backend : public DerivableSingleton<Backend>
{
protected:
    Backend(Str name) : DerivableSingleton(name) {}
public:
    virtual void run(Program *) = 0;

};

#endif // BACKEND_H
