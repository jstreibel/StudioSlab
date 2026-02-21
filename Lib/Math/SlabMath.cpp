//
// Created by joao on 10/13/24.
//

#include "SlabMath.h"
#include "MathModule.h"

#include "Core/Backend/BackendManager.h"
#include "Math/GiNaC/GiNaCModule.h"

namespace Slab::Math {
    void Startup() {
        Core::BackendManager::RegisterAvailableModule<FMathModule>("Math");
        Core::BackendManager::RegisterAvailableModule<GiNaC::FGiNaCModule>("Math:Symbolics");
    }

    void Finish() {

    }

    Vector<FDataRegistry::EntryDescription> EnumerateAllData() {
        return FDataRegistry::GetAllDataEntries();
    }
}
