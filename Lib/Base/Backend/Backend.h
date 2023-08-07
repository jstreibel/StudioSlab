#ifndef BACKEND_H
#define BACKEND_H

#include "Program.h"
#include "Common/Singleton.h"
#include "Events/MouseState.h"
#include "Base/Controller/Interface/InterfaceOwner.h"

class Backend : public DerivableSingleton<Backend>, public InterfaceOwner
{
protected:
    Backend(Str name) : DerivableSingleton(name), InterfaceOwner(Str(name)) {}
public:
    virtual void run(Program *) = 0;

};

#endif // BACKEND_H
