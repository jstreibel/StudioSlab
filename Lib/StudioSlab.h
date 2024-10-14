//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_STUDIOSLAB_H
#define STUDIOSLAB_STUDIOSLAB_H

#include "Core/Backend/BackendManager.h"

namespace Slab {
    void Startup();

    Core::Module &GetModule(Core::ModuleName);
}

#endif //STUDIOSLAB_STUDIOSLAB_H
