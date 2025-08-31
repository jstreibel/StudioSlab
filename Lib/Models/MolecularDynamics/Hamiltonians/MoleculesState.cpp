//
// Created by joao on 10/28/24.
//

#include "MoleculesState.h"

namespace Slab::Models::MolecularDynamics {
    auto FMoleculesState::category() const -> Str {
        return "2nd-order|R->R3N";
    }
} // Slab::Models::MolecularDynamics