//
// Created by joao on 22/09/23.
//

#include "Module.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Core {
    Module::Module(Str name, bool reqGraphBack)
    : name(std::move(name)), requiresGraphicsBackend(reqGraphBack) { }

    Module::~Module() {
        Log::Info() << "Destroying \"" << name << "\" module" << Log::Flush;
    }
} // Core