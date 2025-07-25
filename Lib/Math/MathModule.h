//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_MATHMODULE_H
#define STUDIOSLAB_MATHMODULE_H

#include "Core/Backend/Modules/Module.h"
#include "Math/Data/DataManager.h"

namespace Slab::Math {
    class MathModule : public Core::SlabModule {
    public:
        MathModule();

        void RegisterData(const DataName&, TPointer<Data>);
        DataWrap GetData(const DataName&);
        Vector<DataManager::EntryDescription> GetDataEntries();

    };
} // Slab::Math

#endif //STUDIOSLAB_MATHMODULE_H
