//
// Created by joao on 10/13/24.
//

#include "SlabMath.h"
#include "MathModule.h"

#include "Core/Backend/BackendManager.h"
#include "Math/GiNaC/GiNaCModule.h"

namespace Slab::Math {
    void Register() {
        Core::BackendManager::RegisterAvailableModule<MathModule>("Math");
        Core::BackendManager::RegisterAvailableModule<GiNaC::GiNaCModule>("Math:Symbolics");
    }
}