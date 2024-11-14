//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_MODULE_H
#define STUDIOSLAB_MODULE_H

#include <memory> // Includes Shared Pointer
#include "Utils/String.h"

namespace Slab::Core {

    class Module {
    public:
        const Str name;
        const bool requiresGraphicsBackend;

        Module() = delete;
        explicit Module(Str name, bool requiresGraphicsBackend=false);
        virtual ~Module();
    };

} // Core

#endif //STUDIOSLAB_MODULE_H
