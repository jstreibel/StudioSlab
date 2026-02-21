//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_MATHMODULE_H
#define STUDIOSLAB_MATHMODULE_H

#include "Core/Backend/Modules/Module.h"
#include "Math/Data/DataManager.h"

namespace Slab::Math {
    class FMathModule : public Core::FSlabModule {
    public:
        FMathModule();

        void RegisterData(const FDataName&, TPointer<Data>);
        FDataWrap GetData(const FDataName&);
        Vector<FDataRegistry::EntryDescription> GetDataEntries();

    };

    using MathModule [[deprecated("Use FMathModule")]] = FMathModule;
} // Slab::Math

#endif //STUDIOSLAB_MATHMODULE_H
