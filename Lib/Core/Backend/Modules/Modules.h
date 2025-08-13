//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_MODULES_H
#define STUDIOSLAB_MODULES_H

#include "Utils/String.h"
#include "Utils/Arrays.h"
#include "Module.h"

namespace Slab::Core {
    using FModuleIdentifier = Str;
    using FModuleAllocator = std::function<SlabModule*(void)>;
}

#endif //STUDIOSLAB_MODULES_H
