//
// Created by joao on 22/09/23.
//


#include "BackendManager.h"

#include "Backend.h"
#include "Core/Tools/Log.h"

#include <utility>

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

    /*
    auto BackendManager::GetGUIBackend() -> GraphicBackend & {
        auto &backend = BackendManager::GetBackend();

        if(backend.isHeadless()) throw Exception("requiring graphic backend on headless run");

        return dynamic_cast<GraphicBackend&>(backend);
    }
     */

    void BackendManager::Startup(const BackendName& backend_id) {
        if(BackendManager::instance != nullptr) throw Exception("Backend already initialized");
        if(availableBackends.find(backend_id) == availableBackends.end() )
            throw Exception("Unknown backend '" + backend_id + "'");

        BackendManager::backend_name = backend_id;

        auto alloc_backend = availableBackends[backend_id];
        BackendManager::instance = alloc_backend();
    }

    void BackendManager::LoadModule(const ModuleName& module_name) {
        if(Contains(loadedModules, module_name)) {
            Log::Debug() << "Module '" << module_name << "' already loaded." << Log::Flush;
            return;
        }

        if(availableModules.find(module_name) == availableModules.end())
            throw Exception("Unkonwn module '" + module_name + "'");

        auto alloc_module = availableModules[module_name];
        auto module = Pointer<Module>(alloc_module());

        loadedModules[module_name] = Pointer<Module>(module);

        GetBackend().notifyModuleLoaded(module);

        Log::Info() << "Loaded module '" << Log::FGBlue << module_name << Log::ResetFormatting << "'." << Log::Flush;
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

        Log::Info() << "Backend '" << Log::FGBlue << name << Log::ResetFormatting << "' available." << Log::Flush;
    }

    void BackendManager::RegisterAvailableModule(const ModuleName &name, ModuleAllocator alloc) {
        BackendManager::availableModules[name] = std::move(alloc);

        Log::Info() << "Module '"  << Log::FGBlue << name << Log::ResetFormatting << "' available." << Log::Flush;
    }

    Str BackendManager::GetBackendName() {
        return backend_name;
    }

    void BackendManager::UnloadAllModules() {
        Log::Warning() << "BackendManager was notified to unload all modules, but this is not yet implemented."
                       << Log::Flush;
    }


} // Core