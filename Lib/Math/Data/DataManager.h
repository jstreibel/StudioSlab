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

    enum DataLocation {
        NativeMemory,
        GPU
    };

    class DataManager : public Singleton<DataManager> {
        HashMap<DataName, DataWrap> data_map;

    public:
        struct EntryDescription { DataName name; DataType type; };

        explicit DataManager();

        static DataName
        RegisterData(DataName, Pointer<Data>);

        static void Prune();

        static auto
        GetAllDataEntries()
        -> Vector<EntryDescription>;

        static auto
        AllocFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r, DataLocation)
        -> Pointer<R2toR::NumericFunction>;

        static DataWrap GetData(const DataName&);

        static DataName ChangeDataName(const DataName &old_name, const DataName &new_name);
    };

} // Slab::Math

#endif //STUDIOSLAB_DATAMANAGER_H
