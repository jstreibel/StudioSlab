//
// Created by joao on 10/13/24.
//

#include "MathModule.h"

#include <utility>

#include "Data/DataManager.h"

namespace Slab::Math {
    MathModule::MathModule() : Core::Module("Math") {

    }

    void MathModule::RegisterData(const DataName &name, Pointer<Data> data) {
        DataManager::RegisterData(name, data);
    }

    DataWrap MathModule::GetData(const DataName& name) {
        return DataManager::GetData(name);
    }

    Vector<DataName> MathModule::GetDataEntries() {
        return DataManager::GetAllDataEntries();
    }




} // Slab::Math