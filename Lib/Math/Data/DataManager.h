//
// Created by joao on 26/05/24.
//

#ifndef STUDIOSLAB_DATAMANAGER_H
#define STUDIOSLAB_DATAMANAGER_H

#include "Utils/Singleton.h"
#include "Utils/Map.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Data.h"

// import Maps;

namespace Slab::Math {

    enum DataLocation
    {
        NativeMemory,
        GPU
    };

    class DataRegistry final : public Singleton<DataRegistry>
    {
    public:
        struct EntryDescription { FDataName Name; DataType Type; };

        explicit DataRegistry();

        static FDataName
        RegisterData(FDataName, TPointer<Data>);

        static void Prune();

        static auto
        GetAllDataEntries()
        -> Vector<EntryDescription>;

        static auto
        AllocFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r, DataLocation)
        -> TPointer<R2toR::FNumericFunction>;

        static FDataWrap GetData(const FDataName&);

        static FDataName ChangeDataName(const FDataName &old_name, const FDataName &new_name);

    private:
        HashMap<FDataName, FDataWrap> DataMap;

    };

    class DataKeeper final : public Singleton<DataKeeper>
    {

    public:

        explicit DataKeeper() : Singleton("Data Keeper") { }

        ~DataKeeper() override;
        static TList<TPointer<Data>> GetDataList();

        static void AddData(const TPointer<Data>& Data);

    private:
        TList<TPointer<Data>> DataList;
    };

} // Slab::Math

#endif //STUDIOSLAB_DATAMANAGER_H
