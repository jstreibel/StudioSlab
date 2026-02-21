//
// Created by joao on 10/13/24.
//

#include "MathModule.h"

#include <utility>

#include "Data/DataManager.h"

namespace Slab::Math {
    FMathModule::FMathModule() : Core::FSlabModule("Math") {

    }

    void FMathModule::RegisterData(const FDataName &name, TPointer<Data> data) {
        FDataRegistry::RegisterData(name, data);
    }

    FDataWrap FMathModule::GetData(const FDataName& name) {
        return FDataRegistry::GetData(name);
    }

    Vector<FDataRegistry::EntryDescription> FMathModule::GetDataEntries() {
        return FDataRegistry::GetAllDataEntries();
    }




} // Slab::Math
