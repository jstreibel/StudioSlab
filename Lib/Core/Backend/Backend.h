#ifndef BACKEND_H
#define BACKEND_H

#include "Modules/Modules.h"
#include "Modules/Module.h"

#include "Core/Controller/CommandLine/CommandLineInterfaceOwner.h"



namespace Slab::Core {

    class FBackend : public FCommandLineInterfaceOwner {
        friend class BackendManager;

        Str Name;

        virtual void notifyModuleLoaded(const Pointer<SlabModule>&) {};
    protected:
        explicit FBackend(const Str& Name);

    public:
        ~FBackend() override;
        virtual void Run() = 0;
        virtual void Terminate() = 0;

        [[nodiscard]] Str GetName() const {return Name;}

        [[nodiscard]] virtual auto IsHeadless() const -> bool { return true; }
    };
}

#endif // BACKEND_H
