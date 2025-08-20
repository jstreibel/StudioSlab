//
// Created by joao on 26/05/24.
//

#include "DataManager.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionGPU.h"

namespace Slab::Math {

    FDataRegistry::FDataRegistry() : Singleton("Data Manager") {}

    FDataName FDataRegistry::RegisterData(FDataName name, TPointer<Data> data) {
        if(name.empty()) name = ToStr(data->get_id());

        data->data_name = name;

        Prune();

        auto &map = FDataRegistry::GetInstance().DataMap;

        if(map.contains(name))
            name += " [id:" + ToStr(data->id) + "]";

        if(map.contains(name))
            throw Exception("Corrupted database: some data is using reserved code. Problem with dataset '"
                            + name + "'.");

        map[name] = FDataWrap(data);

        return name;
    }

    TPointer<R2toR::FNumericFunction> FDataRegistry::AllocFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r, DataLocation loc) {
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

    void FDataRegistry::Prune() {
        auto &data_map = GetInstance().DataMap;

        std::erase_if(data_map, [](const auto& pair) {
            const FDataWrap& proxy = pair.second;
            return !proxy.is_valid();
        });
    }

    Vector<FDataRegistry::EntryDescription> FDataRegistry::GetAllDataEntries() {
        Vector<FDataRegistry::EntryDescription> entries;
        for(auto &entry : GetInstance().DataMap)
            entries.emplace_back(EntryDescription(entry.first, entry.second.get_type()) );

        return entries;
    }

    FDataWrap FDataRegistry::GetData(const FDataName& name) {
        auto &data_map = FDataRegistry::GetInstance().DataMap;

        auto entry = data_map.find(name);

        if(entry == data_map.end()) {
            Core::Log::Error("Could not find dataset '" + name + "'.") << Core::Log::Flush;

            static class InvalidData : public Data {
            public:
                explicit InvalidData() : Data("[invalid]") {}
                [[nodiscard]] DataType get_data_type() const override { return "[invalid]"; }
                float get_data_size_MiB() override { return 0; };
            } invalid_data;

            return FDataWrap(Dummy(invalid_data));
        }

        return entry->second;
    }

    FDataName FDataRegistry::ChangeDataName(const FDataName &old_name, const FDataName &new_name) {
        Prune();

        auto &map = GetInstance().DataMap;
        auto entry = map.find(old_name);

        if(entry == map.end()) return "";

        auto proxy = entry->second;

        map.erase(entry->first);

        if(proxy.is_valid()) proxy.GetData()->data_name = new_name;

        return RegisterData(new_name, proxy.GetData());
    }

    FDataManager::~FDataManager() = default;

    void FDataManager::Delete(const TPointer<Data>& Data)
    {
        auto &DataList = GetInstance().DataList;
        DataList.remove(Data);
    }

    TList<TPointer<Data>> FDataManager::GetDataList()
    {
        return FDataManager::GetInstance().DataList;
    }

    void FDataManager::AddData(const TPointer<Data>& Data)
    {
        auto &DataList = FDataManager::GetInstance().DataList;

        if (Contains(DataList, Data)) return;

        DataList.push_back(Data);
    }
} // Slab::Math