//
// Created by joao on 10/13/24.
//

#include "SlabGraphics.h"

#include "Core/Backend/BackendManager.h"
#include "SlabGraphicsModule.h"

#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "Graphics/Backend/SFML/SFMLBackend.h"

#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/Modules/Animator/RealTimeAnimation.h"
#include "Graphics/Modules/Nuklear/NuklearModule.h"
#include "Graphics/Modules/ModernOpenGLModule.h"

namespace Slab::Graphics {

    template<class BackendClass>
    std::unique_ptr<Core::FBackend> BackendAllocator() {
        return std::make_unique<BackendClass>();
    };

    template<class ModuleClass>
    Core::FSlabModule* ModuleAllocator() {
        return new ModuleClass();
    };

    void RegisterBackends() {
        Core::FBackendManager::RegisterAvailableBackend("GLFW",    BackendAllocator<FGLFWBackend>);
        //Core::FBackendManager::RegisterAvailableBackend("GLUT",    BackendAllocator<GLUTBackend>);
        Core::FBackendManager::RegisterAvailableBackend("SFML",    BackendAllocator<FSFMLBackend>);
        Core::FBackendManager::RegisterAvailableBackend("Default", BackendAllocator<FSFMLBackend>);
        Core::FBackendManager::RegisterAvailableBackend("VTK", [] () { throw Exception("VTKBackend not implemented"); return nullptr; }) ;
    }

    void RegisterModules() {
        Core::FBackendManager::RegisterAvailableModule<FSlabGraphicsModule>("Slab:Graphics");

        Core::FBackendManager::RegisterAvailableModule("GUI:ImGui",   [](){ return FImGuiModule::BuildModule(); });
        Core::FBackendManager::RegisterAvailableModule("GUI:Nuklear", [](){ return FNuklearModule::BuildModule(); });
        Core::FBackendManager::RegisterAvailableModule("GUI:NanoGUI", []() {throw Exception("NanoGUI module not implemented"); return nullptr; });

        Core::FBackendManager::RegisterAvailableModule("RealTimeAnimation", ModuleAllocator<FRealTimeAnimationModule>);
        Core::FBackendManager::RegisterAvailableModule("ModernOpenGL",      ModuleAllocator<FModernOpenGLModule>);

        Core::FBackendManager::RegisterAvailableModule("Jack", []() {throw Exception("Jack module not implemented"); return nullptr; });
        Core::FBackendManager::RegisterAvailableModule("NodeJS", []() {throw Exception("NodeJS module not implemented"); return nullptr; });
    }

    void Startup() {
        RegisterBackends();
        RegisterModules();
    }

    void Finish() {

    }

    TPointer<GraphicBackend> GetGraphicsBackend() {
        return DynamicPointerCast<GraphicBackend>(Core::FBackendManager::GetBackend());
    }

}
