//
// Created by joao on 22/09/23.
//


#include "BackendManager.h"

#include <utility>

#include "Backend.h"

#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/SFML/SFMLBackend.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

namespace Slab::Core {
    Str BackendManager::backend_name = "Uninitialized";
    std::unique_ptr<Backend> BackendManager::instance = nullptr;

    Map<ModuleName, ModuleAllocator> BackendManager::availableModules{};
    Map<ModuleName , std::shared_ptr<Module>> BackendManager::loadedModules{};
    Map<BackendName, BackendAllocator> BackendManager::availableBackends{};

    Backend&  BackendManager::GetBackend() {
        if (!BackendManager::instance) throw Exception("Backend must be initialized via BackendManager::Startup before call "
                                             "to BackendManager::GetBackend");

        return *BackendManager::instance;
    }

    auto BackendManager::GetGUIBackend() -> GraphicBackend & {
        auto &backend = BackendManager::GetBackend();

        if(backend.isHeadless()) throw Exception("requiring graphic backend on headless run");

        return dynamic_cast<GraphicBackend&>(backend);
    }

    void BackendManager::Startup(const BackendName& backend_id) {
        if(BackendManager::instance != nullptr) throw Exception("Backend already initialized");
        if(availableBackends.find(backend_id) == availableBackends.end() )
            throw Exception("Unknown backend '" + backend_id + "'");

        BackendManager::backend_name = backend_id;

        auto alloc_backend = availableBackends[backend_id];
        BackendManager::instance = alloc_backend();
    }

    void BackendManager::LoadModule(const ModuleName& module_name) {
        if(loadedModules[module_name] != nullptr) return;

        if(availableModules.find(module_name)  == availableModules.end())
            throw Exception("Unkonwn module '" + module_name + "'");

        auto alloc_module = availableModules[module_name];
        auto module = Pointer<Module>(alloc_module());

        if(module->requiresGraphicsBackend) {
            auto graphModule = DynamicPointerCast<GraphicsModule>(module);

            GetGUIBackend().addGraphicsModule(graphModule);
            loadedModules[module_name] = graphModule;
        } else {
            loadedModules[module_name] = Pointer<Module>(module);
        }
    }

    Module::Ptr BackendManager::GetModule(const ModuleName& module_name) {
        LoadModule(module_name);

        return loadedModules[module_name];
    }

    bool BackendManager::IsModuleLoaded(const ModuleName& module_name) {
        return loadedModules[module_name] != nullptr;
    }

    void BackendManager::RegisterAvailableBackend(const BackendName &name, BackendAllocator alloc) {
        BackendManager::availableBackends[name] = std::move(alloc);
    }

    void BackendManager::RegisterAvailableModule(const ModuleName &name, ModuleAllocator alloc) {
        BackendManager::availableModules[name] = std::move(alloc);
    }

    Str BackendManager::GetBackendName() {
        return backend_name;
    }


} // Core