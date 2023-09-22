//
// Created by joao on 22/09/23.
//


#include "BackendManager.h"

#include "Backend.h"

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Backend/SFML-Nuklear/SFML-Nuklear-Backend.h"
#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/Console/ConsoleBackend.h"
#include "Core/Backend/Modules/ImGuiModule.h"

namespace Core {
    BackendImplementation BackendManager::backendImplementation = Uninitialized;
    std::unique_ptr<Backend> BackendManager::instance = nullptr;

    Backend&  BackendManager::GetBackend() {
        if (!BackendManager::instance) throw Exception("Backend must be initialized via BackendManager::Startup before call "
                                             "to BackendManager::GetBackend");

        return *BackendManager::instance;
    }

    GUIBackend& BackendManager::GetGUIBackend() {
        if (BackendManager::backendImplementation == BackendImplementation::Uninitialized){
            assert(instance == nullptr);
            throw Exception("backend must be initialized via BackendManager::Startup before call to "
                  "BackendManager::GetBackend");
        } else if (BackendManager::backendImplementation == BackendImplementation::Headless) {
            throw Exception("trying to access graphic backend on headless run");
        }

        return *dynamic_cast<GUIBackend*>(instance.get());
    }

    void BackendManager::LoadModule(Modules moduleDescr) {
        auto system = BackendManager::backendImplementation;

        Module *module = nullptr;
        switch (moduleDescr) {
            case ImGui:
                module = new ImGuiModule(system);
                break;
            case Nuklear:
                throw Exception("Nuklear module not implemented");
                break;
            case NanoGUI:
                throw Exception("NanoGUI module not implemented");
                break;
            case Jack:
                throw Exception("Jack module not implemented");
                break;
        }

        auto &guiBackend = GetGUIBackend();
        guiBackend.addModule(std::shared_ptr<Core::Module>(module));
    }

    void BackendManager::Startup(BackendImplementation impl) {
        switch (impl) {
            case Headless:
                BackendManager::instance = std::make_unique<ConsoleBackend>();
            case GLFW:
                BackendManager::instance = std::make_unique<GLFWBackend>();
                break;
            case GLUT:
                BackendManager::instance = std::make_unique<GLUTBackend>();
                break;
            case SFML:
                BackendManager::instance = std::make_unique<SFMLNuklearBackend>();
                break;
            case VTK:
                throw Exception("VTKBackend not implemented");
            default:
                throw Exception("Unknown backend");
        }

        BackendManager::backendImplementation = impl;
    }

    BackendImplementation BackendManager::GetImplementation() { return backendImplementation; }

} // Core