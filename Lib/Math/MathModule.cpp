//
// Created by joao on 10/13/24.
//

#include "MathModule.h"

#include <utility>

#include "Data/DataManager.h"

namespace Slab::Math {
    MathModule::MathModule() : Core::SlabModule("Math") {

    }

    void MathModule::RegisterData(const FDataName &name, TPointer<Data> data) {
        FDataRegistry::RegisterData(name, data);
    }

    FDataWrap MathModule::GetData(const FDataName& name) {
        return FDataRegistry::GetData(name);
    }

    Vector<FDataRegistry::EntryDescription> MathModule::GetDataEntries() {
        return FDataRegistry::GetAllDataEntries();
    }




} // Slab::Math