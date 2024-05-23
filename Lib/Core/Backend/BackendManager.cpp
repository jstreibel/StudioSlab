//
// Created by joao on 22/09/23.
//


#include "BackendManager.h"

#include "Backend.h"

#include "Console/ConsoleBackend.h"
#include "Core/Backend/Modules/ImGui/ImGuiModule.h"
#include "Core/Backend/Modules/RealTimeAnimation.h"
#include "Core/Backend/Modules/ModernOpenGLModule.h"
#include "Core/Backend/Modules/Nuklear/NuklearModule.h"
#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Backend/SFML/SFMLBackend.h"

namespace Slab::Core {
    BackendImplementation BackendManager::backendImplementation = Uninitialized;
    std::unique_ptr<Backend> BackendManager::instance = nullptr;
    std::map<Modules, std::shared_ptr<Module>> BackendManager::loadedModules{};

    Backend&  BackendManager::GetBackend() {
        if (!BackendManager::instance) throw Exception("Backend must be initialized via BackendManager::Startup before call "
                                             "to BackendManager::GetBackend");

        return *BackendManager::instance;
    }

    auto BackendManager::GetGUIBackend() -> GraphicBackend & {
        if(BackendManager::instance->isHeadless()) throw Exception("requiring graphic backend on headless run");

        return dynamic_cast<GraphicBackend&>(GetBackend());
    }

    void BackendManager::Startup(BackendImplementation implementation) {
        if(BackendManager::instance != nullptr) throw Exception("Backend already initialized");

        switch (implementation) {
            case Uninitialized: return;
            case Headless:
                BackendManager::instance = std::make_unique<ConsoleBackend>();
                break;
            case GLFW:
                BackendManager::instance = std::make_unique<GLFWBackend>();
                break;
            case GLUT:
                BackendManager::instance = std::make_unique<GLUTBackend>();
                break;
            case SFML:
                BackendManager::instance = std::make_unique<SFMLBackend>();
                break;
            case VTK:
                throw Exception("VTKBackend not implemented");
            default:
                throw Exception("Unknown backend");
        }

        BackendManager::backendImplementation = implementation;
    }



    void BackendManager::LoadModule(Modules moduleDescr) {
        if(loadedModules[moduleDescr] != nullptr) return;

        fix system = BackendManager::backendImplementation;

        Module *module = nullptr;
        switch (moduleDescr) {
            case ImGui:
                module = ImGuiModule::BuildModule(system);      break;
            case RealTimeAnimation:
                module = new RealTimeAnimationModule;           break;
            case Nuklear:
                module = NuklearModule::BuildModule(system);    break;
            case ModernOpenGL:
                module = new ModernOpenGLModule();              break;
            case NanoGUI:
                throw Exception("NanoGUI module not implemented");
            case Jack:
                throw Exception("Jack module not implemented");
            case NodeJS:
                throw Exception("NodeJS module not implemented");
        }

        Module::Ptr modulePtr = Module::Ptr(module);
        auto &guiBackend = GetGUIBackend();
        guiBackend.addModule(modulePtr);

        loadedModules[moduleDescr] = modulePtr;
   }


    Module::Ptr BackendManager::GetModule(Modules moduleDescr) {
        LoadModule(moduleDescr);

        return loadedModules[moduleDescr];
    }

    bool BackendManager::IsModuleLoaded(Modules moduleDescr) {
        return loadedModules[moduleDescr] != nullptr;
    }



} // Core