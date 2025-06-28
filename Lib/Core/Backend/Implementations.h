//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMPLEMENTATIONS_H
#define STUDIOSLAB_IMPLEMENTATIONS_H

#include "Utils/String.h"
#include "Core/Backend/Backend.h"

#include <functional>

namespace Slab::Core {

    using FBackendIdentifier = Str;

    using FBackendAllocator = std::function<std::unique_ptr<FBackend>(void)>;
}

#endif //STUDIOSLAB_IMPLEMENTATIONS_H
