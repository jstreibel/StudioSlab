#ifndef BACKEND_H
#define BACKEND_H

#include "Core/Backend/Modules/TaskManager/Task.h"
#include "Modules/Modules.h"
#include "Modules/Module.h"

#include "Core/Controller/CommandLine/CLInterfaceOwner.h"



namespace Slab::Core {

    class Backend : public CLInterfaceOwner {
        Str name;

    protected:
        explicit Backend(const Str& name)
        : CLInterfaceOwner(Str(name)) {}

    public:
        virtual ~Backend() = default;
        virtual void run() = 0;
        virtual void terminate() = 0;

        Str getName() const {return name;}

        virtual auto isHeadless() const -> bool { return true; }

    };
}

#endif // BACKEND_H
