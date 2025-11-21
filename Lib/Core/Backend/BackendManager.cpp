//
// Created by joao on 22/09/23.
//


#include "BackendManager.h"

#include "Backend.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Core {
    Str BackendManager::BackendName = "Uninitialized";
    std::shared_ptr<FBackend> BackendManager::instance = nullptr;

    Map<FModuleIdentifier, FModuleAllocator> BackendManager::AvailableModules{};
    Map<FModuleIdentifier , std::shared_ptr<SlabModule>> BackendManager::LoadedModules{};
    Map<FBackendIdentifier, FBackendAllocator> BackendManager::AvailableBackends{};

    TPointer<FBackend>  BackendManager::GetBackend() {
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

    void BackendManager::Startup(const FBackendIdentifier& backend_id) {
        if(BackendManager::instance != nullptr)
            throw Exception("Backend already initialized");
        if(AvailableBackends.find(backend_id) == AvailableBackends.end() )
            throw Exception("Unknown backend '" + backend_id + "'");

        BackendManager::BackendName = backend_id;

        auto alloc_backend = AvailableBackends[backend_id];
        BackendManager::instance = alloc_backend();
    }

    void BackendManager::LoadModule(const FModuleIdentifier& module_name) {
        if(IsModuleLoaded(module_name)) return;

        if(!IsModuleAvailable(module_name)) throw Exception("Unkonwn module '" + module_name + "'");

        auto alloc_module = AvailableModules[module_name];
        auto module = TPointer<SlabModule>(alloc_module());

        LoadedModules[module_name] = TPointer<SlabModule>(module);

        GetBackend()->NotifyModuleLoaded(module);

        Log::Info() << "Loaded module '" << Log::FGBlue << module_name << Log::ResetFormatting << "'." << Log::Flush;
    }

    FModuleIdentifier BackendManager::ParseName(const FModuleIdentifier& requested_module_name) {
        if(IsModuleAvailable(requested_module_name)) return requested_module_name;

        auto split_reqmod_name = Split(requested_module_name, ":");
        if(split_reqmod_name.size() != 1) goto bad;

        for(const auto& pair : AvailableModules) {
            auto name = pair.first;

            auto split_name = Split(name, ":");

            if(split_name.size() == 1) continue;

            // The first registered module of this kind is the default.
            // For example, from {'GUI:ImGui', 'GUI:Nuklear', 'GUI:Nanogui'}, the module returned for the 'GUI' request
            // is the first, 'GUI:ImGui'.
            if(Contains(split_name, requested_module_name)) return name;
        }

        bad:
        Log::Warning() << "Module '" << Log::FGBlue << requested_module_name << Log::ResetFormatting << "' not found." << Log::Flush;
        Log::Info() << "Available modules:" << Log::Flush;
        for(const auto& pair : AvailableModules) Log::Info() << "\t" << pair.first << Log::Flush;
        throw Exception("Unknown module '" + requested_module_name + "'");
    }

    StrVector BackendManager::GetAvailableModules() {
        StrVector modules;
        for(const auto& pair : AvailableModules) modules.push_back(pair.first);
        return modules;
    }

    TPointer<SlabModule> BackendManager::GetModule(const FModuleIdentifier& module_name) {
        auto parsed_name = ParseName(module_name);

        if(!IsModuleLoaded(parsed_name)) LoadModule(parsed_name);

        return LoadedModules[parsed_name];
    }
    bool BackendManager::IsModuleAvailable(const FModuleIdentifier &module_name) {
        return Contains(AvailableModules, module_name);
    }

    bool BackendManager::IsModuleLoaded(const FModuleIdentifier& module_name) {
        return Contains(LoadedModules, module_name);
    }

    void BackendManager::RegisterAvailableBackend(const FBackendIdentifier &Name, FBackendAllocator Alloc) {

        BackendManager::AvailableBackends[Name] = std::move(Alloc);

        Log::Note() << "Backend '" << Log::FGBlue << Name << Log::ResetFormatting << "' available." << Log::Flush;
    }

    void BackendManager::RegisterAvailableModule(const FModuleIdentifier &Name, FModuleAllocator Alloc) {
        BackendManager::AvailableModules[Name] = std::move(Alloc);

        Log::Note() << "Module '"  << Log::FGBlue << Name << Log::ResetFormatting << "' available." << Log::Flush;
    }

    Str BackendManager::GetBackendName() {
        return BackendName;
    }

    void BackendManager::UnloadAllModules() {
        LoadedModules.clear();
    }


} // Core