//
// Created by joao on 10/13/24.
//

#include "SlabMath.h"
#include "MathModule.h"

#include "Core/Backend/BackendManager.h"
#include "Math/GiNaC/GiNaCModule.h"

namespace Slab::Math {
    void Startup() {
        Core::BackendManager::RegisterAvailableModule<MathModule>("Math");
        Core::BackendManager::RegisterAvailableModule<GiNaC::GiNaCModule>("Math:Symbolics");
    }

    void Finish() {

    }

    Vector<FDataRegistry::EntryDescription> EnumerateAllData() {
        return FDataRegistry::GetAllDataEntries();
    }
}