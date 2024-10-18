//
// Created by joao on 10/13/24.
//

#include "SlabGraphics.h"

#include "Core/Backend/BackendManager.h"
#include "GraphicsModule.h"

#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "Graphics/Backend/GLUT/GLUTBackend.h"
#include "Graphics/Backend/SFML/SFMLBackend.h"

#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/Modules/Animator/RealTimeAnimation.h"
#include "Graphics/Modules/Nuklear/NuklearModule.h"
#include "Graphics/Modules/ModernOpenGLModule.h"

namespace Slab::Graphics {

    template<class BackendClass>
    std::unique_ptr<Core::Backend> BackendAllocator() {
        return std::make_unique<BackendClass>();
    };

    template<class ModuleClass>
    Core::Module* ModuleAllocator() {
        return new ModuleClass();
    };

    void Register() {
        Core::BackendManager::RegisterAvailableModule<SlabGraphicsModule>("Slab:Graphics");

        Core::BackendManager::RegisterAvailableBackend("GLFW",    BackendAllocator<Core::GLFWBackend>);
        Core::BackendManager::RegisterAvailableBackend("GLUT",    BackendAllocator<Core::GLUTBackend>);
        Core::BackendManager::RegisterAvailableBackend("SFML",    BackendAllocator<Core::SFMLBackend>);
        Core::BackendManager::RegisterAvailableBackend("Default", BackendAllocator<Core::SFMLBackend>);
        Core::BackendManager::RegisterAvailableBackend("VTK", [] () { throw Exception("VTKBackend not implemented"); return nullptr; }) ;

        Core::BackendManager::RegisterAvailableModule("ImGui",   [](){ return Core::ImGuiModule::BuildModule(); });
        Core::BackendManager::RegisterAvailableModule("Nuklear", [](){ return Core::NuklearModule::BuildModule(); });
        Core::BackendManager::RegisterAvailableModule("NanoGUI", []() {throw Exception("NanoGUI module not implemented"); return nullptr; });

        Core::BackendManager::RegisterAvailableModule("RealTimeAnimation", ModuleAllocator<Core::RealTimeAnimationModule>);
        Core::BackendManager::RegisterAvailableModule("ModernOpenGL",      ModuleAllocator<Core::ModernOpenGLModule>);

        Core::BackendManager::RegisterAvailableModule("Jack", []() {throw Exception("Jack module not implemented"); return nullptr; });
        Core::BackendManager::RegisterAvailableModule("NodeJS", []() {throw Exception("NodeJS module not implemented"); return nullptr; });
    }

    void Finish() {

    }

    Core::GraphicBackend& GetGraphicsBackend() {
        return dynamic_cast<Core::GraphicBackend&>(Core::BackendManager::GetBackend());
    }

}