//
// Created by joao on 10/16/24.
//

#include "MolDynNumericConfig.h"

namespace Slab::Models::MolecularDynamics {
    MolDynNumericConfig::MolDynNumericConfig(bool do_register)
    : DynamicsNumericConfig(false)
    {
        Interface->addParameters({n});

        if(do_register) registerToManager();
    }

    UInt MolDynNumericConfig::getn() const {
        return **n;
    }

    Str MolDynNumericConfig::to_string() const {
        NOT_IMPLEMENTED
        return Slab::Str();
    }

    auto MolDynNumericConfig::getdt() const -> Real {
        return **t / **n;
    }

} // Slab::Models::MolecularDynamics