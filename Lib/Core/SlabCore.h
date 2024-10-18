//
// Created by joao on 10/11/24.
//

#ifndef STUDIOSLAB_SLABCORE_H
#define STUDIOSLAB_SLABCORE_H

#include "Backend/BackendManager.h"

namespace Slab::Core {
    Backend &GetBackend();

    void LoadModule(const ModuleName&);
    Pointer<Module> GetModule(const ModuleName&);

    void Register();
    void Finish();
}

#endif //STUDIOSLAB_SLABCORE_H
