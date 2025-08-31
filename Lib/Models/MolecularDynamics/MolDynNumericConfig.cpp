//
// Created by joao on 10/16/24.
//

#include "MolDynNumericConfig.h"

namespace Slab::Models::MolecularDynamics {
    MolDynNumericConfig::MolDynNumericConfig(bool do_register)
    : DynamicsNumericConfig(false)
    {
        Interface->AddParameters({n});

        if(do_register) RegisterToManager();
    }

    UInt MolDynNumericConfig::Get_n() const {
        return **n;
    }

    Str MolDynNumericConfig::to_string() const {
        NOT_IMPLEMENTED
        return Slab::Str();
    }

    auto MolDynNumericConfig::getdt() const -> DevFloat {
        return **t / **n;
    }

} // Slab::Models::MolecularDynamics