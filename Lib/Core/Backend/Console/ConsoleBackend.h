#ifndef CONSOLEBACKEND_H
#define CONSOLEBACKEND_H

#include "Core/Backend/Backend.h"

namespace Slab::Core {

    class FConsoleBackend : public FBackend {
    public:
        FConsoleBackend();

        ~FConsoleBackend() override;

        void Run() override;

        void Terminate() override;
    };
}

#endif // CONSOLEBACKEND_H
