//
// Created by joao on 10/11/24.
//

#ifndef STUDIOSLAB_BASE_H
#define STUDIOSLAB_BASE_H

#include "Backend/BackendManager.h"

namespace Slab::Core {
    Backend &GetBackend();
    GraphicBackend &GetGUIBackend();

    void Register();
}

#endif //STUDIOSLAB_BASE_H
