#ifndef BACKEND_H
#define BACKEND_H

#include "Program/Program.h"
#include "Modules/Modules.h"
#include "Modules/Module.h"

#include "Core/Controller/Interface/InterfaceOwner.h"



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

        Str getName() const {return name;}

    };
}

#endif // BACKEND_H
