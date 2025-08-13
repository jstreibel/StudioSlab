//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_MODULE_H
#define STUDIOSLAB_MODULE_H

#include "Utils/String.h"

namespace Slab::Core {

    class SlabModule {
    public:
        const Str Name;
        const bool bRequiresGraphicsBackend;

        SlabModule() = delete;
        explicit SlabModule(Str name, bool bRequiresGraphicsBackend=false);
        virtual ~SlabModule();
    };

} // Core

#endif //STUDIOSLAB_MODULE_H
