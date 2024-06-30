#ifndef CONSOLEBACKEND_H
#define CONSOLEBACKEND_H

#include "Core/Backend/Backend.h"

namespace Slab::Core {

    class ConsoleBackend : public Backend {
    public:
        ConsoleBackend();

        ~ConsoleBackend() override;

        void run() override;

        void terminate() override;
    };
}

#endif // CONSOLEBACKEND_H
