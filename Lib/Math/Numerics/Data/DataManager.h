//
// Created by joao on 26/05/24.
//

#ifndef STUDIOSLAB_DATAMANAGER_H
#define STUDIOSLAB_DATAMANAGER_H

#include "Utils/Singleton.h"
#include "Utils/Map.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "DataSet.h"

// import Maps;

namespace Slab::Math {

    enum DataLocation {
        NativeMemory,
        GPU
    };

    using DataSetType = Str;
    using DataSetName = Str;
    using DataSetEntry = Pair<DataSetType, DataSet_ptr>;

    class DataManager : public Singleton<DataManager> {
        Map<DataSetName, DataSet_ptr> dataSets;

    public:
        explicit DataManager();

        static void AddData(DataSetName, Pointer<DataSet>);
        static Vector<DataSetName> GetAllDataEntries();
        static R2toR::NumericFunction_ptr NewFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r, DataLocation);
        static DataSetEntry GetData(const DataSetName&);
    };

} // Slab::Math

#endif //STUDIOSLAB_DATAMANAGER_H
