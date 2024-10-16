//
// Created by joao on 26/05/24.
//

#include "DataManager.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionGPU.h"

namespace Slab::Math {

    DataManager::DataManager() : Singleton("Data Manager") {}

    void DataManager::AddData(DataSetName name, Pointer<DataSet> dataSet) {
        DataManager::GetInstance().dataSets[name] = dataSet;
    }

    R2toR::NumericFunction_ptr DataManager::NewFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r, DataLocation loc) {
        fix hx = r.x/(Real)N;
        fix hy = r.y/(Real)M;

/*
        Pointer<R2toR::NumericFunction> data;
        switch (loc) {
            case NativeMemory:
                data = Slab::New<R2toR::NumericFunction_CPU>(N, M, rMin.x, rMin.y, hx, hy);
                break;
            case GPU:
                #if USE_CUDA
                    data = Slab::New<R2toR::NumericFunction_GPU>(N, M, rMin.x, rMin.y, hx, hy);
                #else
                    throw NotImplementedException("CUDA implementation @ " + Str(__PRETTY_FUNCTION__));
                #endif
                break;
        }

        auto dataSet = New<DataSet>(data, "R2toR::NumericFunction");

        GetInstance().dataSets[uniqueName] = dataSet;

        return data;
        */

        NOT_IMPLEMENTED
    }

    Vector<DataSetName> DataManager::GetAllDataEntries() {
        Vector<DataSetName> entries;
        for(auto &pair : GetInstance().dataSets)
            entries.emplace_back(pair.first);

        return entries;
    }

    DataSetEntry DataManager::GetData(const DataSetName& name) {
        auto &sets = DataManager::GetInstance().dataSets;
        auto entry = sets.find(name);

        if(entry == sets.end())
            Core::Log::Error("Could not find dataset '" + name + "'.") << Core::Log::Flush;

        return *entry;
    }

} // Slab::Math