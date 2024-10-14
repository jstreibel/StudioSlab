//
// Created by joao on 10/13/24.
//

#include "MathModule.h"

#include "Numerics/Data/DataManager.h"

namespace Slab::Math {
    MathModule::MathModule() : Core::Module("Math") {

    }

    void MathModule::RegisterData(const DataSetName &name, Pointer<DataSet> dataSet) {
        DataManager::AddData(name, dataSet);
    }

    DataSetEntry MathModule::GetData(const DataSetName& name) {
        return DataManager::GetData(name);
    }

    Vector<DataSetName> MathModule::GetDataEntries() {
        return DataManager::GetAllDataEntries();
    }




} // Slab::Math