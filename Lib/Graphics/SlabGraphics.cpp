//
// Created by joao on 10/13/24.
//

#include "SlabGraphics.h"

#include "Core/Backend/BackendManager.h"
#include "GraphicsModule.h"

#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "Graphics/Backend/GLUT/GLUTBackend.h"
#include "Graphics/Backend/SFML/SFMLBackend.h"

namespace Slab::Graphics {

    template<class BackendClass>
    std::unique_ptr<Core::Backend> Allocator() {
        return std::make_unique<BackendClass>();
    };

    void Register() {
        Core::BackendManager::RegisterAvailableModule<SlabGraphicsModule>("Slab:Graphics");

        Core::BackendManager::RegisterAvailableBackend("GLFW", Allocator<Core::GLFWBackend>);
        Core::BackendManager::RegisterAvailableBackend("GLUT", Allocator<Core::GLUTBackend>);
        Core::BackendManager::RegisterAvailableBackend("SFML", Allocator<Core::SFMLBackend>);
        Core::BackendManager::RegisterAvailableBackend("Default", Allocator<Core::SFMLBackend>);
        Core::BackendManager::RegisterAvailableBackend("VTK", [] () { throw Exception("VTKBackend not implemented"); return nullptr; }) ;
    }

    void Finish() {

    }

    Core::GraphicBackend& GetGraphicsBackend() {
        return dynamic_cast<Core::GraphicBackend&>(Core::BackendManager::GetBackend());
    }

}