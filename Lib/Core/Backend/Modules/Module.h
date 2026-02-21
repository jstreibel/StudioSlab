//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_MODULE_H
#define STUDIOSLAB_MODULE_H

#include "Utils/String.h"

namespace Slab::Core {

    class FSlabModule {
    public:
        const Str Name;
        const bool bRequiresGraphicsBackend;

        FSlabModule() = delete;
        explicit FSlabModule(Str name, bool bRequiresGraphicsBackend = false);
        virtual ~FSlabModule();
    };

    using SlabModule [[deprecated("Use FSlabModule")]] = FSlabModule;

} // Core

#endif //STUDIOSLAB_MODULE_H
