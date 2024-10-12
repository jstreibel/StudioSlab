//
// Created by joao on 04/06/23.
//

#include "Base.h"

Slab::Core::Backend &Slab::Core::GetBackend() {
    return BackendManager::GetBackend();
}

Slab::Core::GraphicBackend &Slab::Core::GetGUIBackend() {
    return BackendManager::GetGUIBackend();
}
