#ifndef BACKEND_H
#define BACKEND_H

#include "Program/Program.h"
#include "Core/Controller/Interface/InterfaceOwner.h"

#include "Modules/Modules.h"
#include "Core/Backend/Modules/Module.h"

namespace Core {

    class Backend : public InterfaceOwner {
        Str name;

    protected:
        explicit Backend(const Str& name)
        : InterfaceOwner(Str(name)) {}

    public:
        virtual ~Backend() = default;
        virtual void run(Program *) = 0;
        virtual void finish() = 0;

    };
}

#endif // BACKEND_H
