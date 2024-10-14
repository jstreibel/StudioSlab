//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_MATHMODULE_H
#define STUDIOSLAB_MATHMODULE_H

#include "Core/Backend/Modules/Module.h"
#include "Math/Numerics/Data/DataManager.h"

namespace Slab::Math {
    class MathModule : public Core::Module {
    public:
        MathModule();

        void RegisterData(const DataSetName&, Pointer<DataSet> dataSet);
        DataSetEntry GetData(const DataSetName&);
        Vector<DataSetName> GetDataEntries();

    };
} // Slab::Math

#endif //STUDIOSLAB_MATHMODULE_H
