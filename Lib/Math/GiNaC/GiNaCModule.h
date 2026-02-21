//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_GINACMODULE_H
#define STUDIOSLAB_GINACMODULE_H

#include "Core/Backend/Modules/Module.h"

namespace Slab::Math::GiNaC {

    class FGiNaCModule : public Core::FSlabModule {
    public:
        FGiNaCModule();
    };

    using GiNaCModule [[deprecated("Use FGiNaCModule")]] = FGiNaCModule;

} // Slab::Math::GiNaC

#endif //STUDIOSLAB_GINACMODULE_H
