//
// Created by joao on 11/05/24.
//

#include "GraphicBackendManager.h"

namespace Graphics {
} // Graphics


GraphicBackend& BackendManager::GetGUIBackend() {
    if (BackendManager::backendImplementation == BackendImplementation::Uninitialized){
        Startup(DefaultGUIBackend);
    } else if (BackendManager::backendImplementation == BackendImplementation::Headless) {
        throw Exception("trying to access graphic backend on headless run");
    }

    return *dynamic_cast<GraphicBackend*>(instance.get());
}