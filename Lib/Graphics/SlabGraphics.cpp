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
    Core::SlabModule* ModuleAllocator() {
        return new ModuleClass();
    };

    void RegisterBackends() {
        Core::BackendManager::RegisterAvailableBackend("GLFW",    BackendAllocator<FGLFWBackend>);
        //Core::BackendManager::RegisterAvailableBackend("GLUT",    BackendAllocator<GLUTBackend>);
        Core::BackendManager::RegisterAvailableBackend("SFML",    BackendAllocator<FSFMLBackend>);
        Core::BackendManager::RegisterAvailableBackend("Default", BackendAllocator<FSFMLBackend>);
        Core::BackendManager::RegisterAvailableBackend("VTK", [] () { throw Exception("VTKBackend not implemented"); return nullptr; }) ;
    }

    void RegisterModules() {
        Core::BackendManager::RegisterAvailableModule<FSlabGraphicsModule>("Slab:Graphics");

        Core::BackendManager::RegisterAvailableModule("GUI:ImGui",   [](){ return FImGuiModule::BuildModule(); });
        Core::BackendManager::RegisterAvailableModule("GUI:Nuklear", [](){ return FNuklearModule::BuildModule(); });
        Core::BackendManager::RegisterAvailableModule("GUI:NanoGUI", []() {throw Exception("NanoGUI module not implemented"); return nullptr; });

        Core::BackendManager::RegisterAvailableModule("RealTimeAnimation", ModuleAllocator<FRealTimeAnimationModule>);
        Core::BackendManager::RegisterAvailableModule("ModernOpenGL",      ModuleAllocator<FModernOpenGLModule>);

        Core::BackendManager::RegisterAvailableModule("Jack", []() {throw Exception("Jack module not implemented"); return nullptr; });
        Core::BackendManager::RegisterAvailableModule("NodeJS", []() {throw Exception("NodeJS module not implemented"); return nullptr; });
    }

    void Startup() {
        RegisterBackends();
        RegisterModules();
    }

    void Finish() {

    }

    TPointer<GraphicBackend> GetGraphicsBackend() {
        return DynamicPointerCast<GraphicBackend>(Core::BackendManager::GetBackend());
    }

}
