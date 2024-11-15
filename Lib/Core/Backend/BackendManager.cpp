//
// Created by joao on 22/09/23.
//


#include "BackendManager.h"

#include "Backend.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Core {
    Str BackendManager::backend_name = "Uninitialized";
    std::shared_ptr<Backend> BackendManager::instance = nullptr;

    Map<ModuleName, ModuleAllocator> BackendManager::availableModules{};
    Map<ModuleName , std::shared_ptr<Module>> BackendManager::loadedModules{};
    Map<BackendName, BackendAllocator> BackendManager::availableBackends{};

    Pointer<Backend>  BackendManager::GetBackend() {
        if (!BackendManager::instance) {
            BackendManager::Startup("Headless");
            Log::Info() << "Backend initializing to default headless backend." << Log::Flush;
        };

        return BackendManager::instance;
    }

    /*
    auto BackendManager::GetGUIBackend() -> GraphicBackend & {
        auto &backend = BackendManager::GetBackend();

        if(backend.isHeadless()) throw Exception("requiring graphic backend on headless run");

        return dynamic_cast<GraphicBackend&>(backend);
    }
     */

    void BackendManager::Startup(const BackendName& backend_id) {
        if(BackendManager::instance != nullptr)
            throw Exception("Backend already initialized");
        if(availableBackends.find(backend_id) == availableBackends.end() )
            throw Exception("Unknown backend '" + backend_id + "'");

        BackendManager::backend_name = backend_id;

        auto alloc_backend = availableBackends[backend_id];
        BackendManager::instance = alloc_backend();
    }

    void BackendManager::LoadModule(const ModuleName& module_name) {
        if(IsModuleLoaded(module_name)) return;

        if(!IsModuleAvailable(module_name)) throw Exception("Unkonwn module '" + module_name + "'");

        auto alloc_module = availableModules[module_name];
        auto module = Pointer<Module>(alloc_module());

        loadedModules[module_name] = Pointer<Module>(module);

        GetBackend()->notifyModuleLoaded(module);

        Log::Info() << "Loaded module '" << Log::FGBlue << module_name << Log::ResetFormatting << "'." << Log::Flush;
    }

    ModuleName BackendManager::ParseName(const ModuleName& requested_module_name) {
        if(IsModuleAvailable(requested_module_name)) return requested_module_name;

        auto split_reqmod_name = Split(requested_module_name, ":");
        if(split_reqmod_name.size() != 1) goto bad;

        for(const auto& pair : availableModules) {
            auto name = pair.first;

            auto split_name = Split(name, ":");

            if(split_name.size() == 1) continue;

            // The first registered module of this kind is the default.
            // For example, from {'GUI:ImGui', 'GUI:Nuklear', 'GUI:Nanogui'}, the module returned for the 'GUI' request
            // is the first, 'GUI:ImGui'.
            if(Contains(split_name, requested_module_name)) return name;
        }

        bad:
        throw Exception("Unknown module '" + requested_module_name + "'");
    }

    Pointer<Module> BackendManager::GetModule(const ModuleName& module_name) {
        auto parsed_name = ParseName(module_name);

        if(!IsModuleLoaded(parsed_name)) LoadModule(parsed_name);

        return loadedModules[parsed_name];
    }
    bool BackendManager::IsModuleAvailable(const ModuleName &module_name) {
        return Contains(availableModules, module_name);
    }

    bool BackendManager::IsModuleLoaded(const ModuleName& module_name) {
        return Contains(loadedModules, module_name);
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
        loadedModules.clear();
    }


} // Core