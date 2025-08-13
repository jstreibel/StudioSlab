//
// Created by joao on 22/09/23.
//

#include "Module.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Core {
    SlabModule::SlabModule(Str name, bool reqGraphBack)
    : Name(std::move(name)), bRequiresGraphicsBackend(reqGraphBack) { }

    SlabModule::~SlabModule() {
        Log::Info() << "Releasing module \'" << Log::FGBlue << Name << Log::ResetFormatting << "\'." << Log::Flush;
    }
} // Core