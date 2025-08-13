//
// Created by joao on 15/12/24.
//

#include "Backend.h"


namespace Slab::Core {

    FBackend::FBackend(const Str &Name) : FCommandLineInterfaceOwner(Str(Name)) {
    }

    FBackend::~FBackend() = default;

}
