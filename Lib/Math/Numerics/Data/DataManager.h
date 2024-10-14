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

    class DataManager : public Singleton<DataManager> {
        using DataSetType = Str;
        using DataSetName = Str;
        typedef Pair<DataSetType, DataSet_ptr> DataSetEntry;

        Map<DataSetName, DataSet_ptr> dataSets;

    public:
        explicit DataManager();

        static Vector<DataSetName> GetAllDataEntries();
        static R2toR::NumericFunction_ptr NewFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r, DataLocation);
    };

} // Slab::Math

#endif //STUDIOSLAB_DATAMANAGER_H
