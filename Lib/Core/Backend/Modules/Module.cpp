//
// Created by joao on 22/09/23.
//

#include "Module.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Core {
    FSlabModule::FSlabModule(Str name, bool reqGraphBack)
    : Name(std::move(name)), bRequiresGraphicsBackend(reqGraphBack) { }

    FSlabModule::~FSlabModule() {
        FLog::Info() << "Releasing module \'" << FLog::FGBlue << Name << FLog::ResetFormatting << "\'." << FLog::Flush;
    }
} // Core
