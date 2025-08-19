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
        DataRegistry::RegisterData(name, data);
    }

    FDataWrap MathModule::GetData(const FDataName& name) {
        return DataRegistry::GetData(name);
    }

    Vector<DataRegistry::EntryDescription> MathModule::GetDataEntries() {
        return DataRegistry::GetAllDataEntries();
    }




} // Slab::Math