//
// Created by joao on 26/05/24.
//

#ifndef STUDIOSLAB_DATAMANAGER_H
#define STUDIOSLAB_DATAMANAGER_H

#include "Utils/Singleton.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "DataSet.h"

// import Maps;

namespace Slab::Math {

    class DataManager : public Singleton<DataManager> {
        typedef Pair<Str, DataSet_ptr> DataSetEntry;
        // Map<DataSetEntry> dataSets;

    public:
        explicit DataManager();

        static R2toR::NumericFunction_ptr NewFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r);
    };

} // Slab::Math

#endif //STUDIOSLAB_DATAMANAGER_H
