//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMPLEMENTATIONS_H
#define STUDIOSLAB_IMPLEMENTATIONS_H

#include "Utils/String.h"
#include "Core/Backend/Backend.h"

#include <functional>

namespace Slab::Core {

    using BackendName = Str;

    using BackendAllocator = std::function<std::unique_ptr<Backend>(void)>;
}

#endif //STUDIOSLAB_IMPLEMENTATIONS_H
