//
// Created by joao on 26/05/24.
//

#include "DataManager.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionGPU.h"

namespace Slab::Math {

    DataManager::DataManager() : Singleton("Data Manager") {}

    DataName DataManager::RegisterData(DataName name, Pointer<Data> data) {
        if(name.empty()) name = ToStr(data->get_id());

        data->data_name = name;

        Prune();

        auto &map = DataManager::GetInstance().data_map;

        if(map.find(name) != map.end())
            name += " [id:" + ToStr(data->id) + "]";

        if(map.find(name) != map.end())
            throw Exception("Corrupted database: some data is using reserved code. Problem with dataset '"
                            + name + "'.");

        map[name] = DataWrap(data);

        return name;
    }

    Pointer<R2toR::NumericFunction> DataManager::AllocFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r, DataLocation loc) {
        fix hx = r.x/(DevFloat)N;
        fix hy = r.y/(DevFloat)M;

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

    void DataManager::Prune() {
        auto &data_map = GetInstance().data_map;

        std::erase_if(data_map, [](const auto& pair) {
            const DataWrap& proxy = pair.second;
            return !proxy.is_valid();
        });
    }

    Vector<DataManager::EntryDescription> DataManager::GetAllDataEntries() {
        Vector<DataManager::EntryDescription> entries;
        for(auto &entry : GetInstance().data_map)
            entries.emplace_back(EntryDescription(entry.first, entry.second.get_type()) );

        return entries;
    }

    DataWrap DataManager::GetData(const DataName& name) {
        auto &data_map = DataManager::GetInstance().data_map;

        auto entry = data_map.find(name);

        if(entry == data_map.end()) {
            Core::Log::Error("Could not find dataset '" + name + "'.") << Core::Log::Flush;

            static class InvalidData : public Data {
            public:
                explicit InvalidData() : Data("[invalid]") {}
                DataType get_data_type() const override { return "[invalid]"; }
            } invalid_data;

            return DataWrap(Dummy(invalid_data));
        }

        return entry->second;
    }

    DataName DataManager::ChangeDataName(const DataName &old_name, const DataName &new_name) {
        Prune();

        auto &map = GetInstance().data_map;
        auto entry = map.find(old_name);

        if(entry == map.end()) return "";

        auto proxy = entry->second;

        map.erase(entry->first);

        if(proxy.is_valid()) proxy.get_data()->data_name = new_name;

        return RegisterData(new_name, proxy.get_data());
    }


} // Slab::Math