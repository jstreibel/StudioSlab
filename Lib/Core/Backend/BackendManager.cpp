//
// Created by joao on 22/09/23.
//


#include "BackendManager.h"

#include "Backend.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Core {
    Str FBackendManager::BackendName = "Uninitialized";
    std::shared_ptr<FBackend> FBackendManager::instance = nullptr;

    Map<FModuleIdentifier, FModuleAllocator> FBackendManager::AvailableModules{};
    Map<FModuleIdentifier , std::shared_ptr<FSlabModule>> FBackendManager::LoadedModules{};
    Map<FBackendIdentifier, FBackendAllocator> FBackendManager::AvailableBackends{};

    TPointer<FBackend>  FBackendManager::GetBackend() {
        if (!FBackendManager::instance) {
            FBackendManager::Startup("Headless");
            FLog::Info() << "Backend initializing to default headless backend." << FLog::Flush;
        };

        return FBackendManager::instance;
    }

    /*
    auto FBackendManager::GetGUIBackend() -> GraphicBackend & {
        auto &backend = FBackendManager::GetBackend();

        if(backend.isHeadless()) throw Exception("requiring graphic backend on headless run");

        return dynamic_cast<GraphicBackend&>(backend);
    }
     */

    void FBackendManager::Startup(const FBackendIdentifier& backend_id) {
        if(FBackendManager::instance != nullptr)
            throw Exception("Backend already initialized");
        if(AvailableBackends.find(backend_id) == AvailableBackends.end() )
            throw Exception("Unknown backend '" + backend_id + "'");

        FBackendManager::BackendName = backend_id;

        auto alloc_backend = AvailableBackends[backend_id];
        FBackendManager::instance = alloc_backend();
    }

    void FBackendManager::LoadModule(const FModuleIdentifier& module_name) {
        if(IsModuleLoaded(module_name)) return;

        if(!IsModuleAvailable(module_name)) throw Exception("Unkonwn module '" + module_name + "'");

        auto alloc_module = AvailableModules[module_name];
        auto module = TPointer<FSlabModule>(alloc_module());

        LoadedModules[module_name] = TPointer<FSlabModule>(module);

        GetBackend()->NotifyModuleLoaded(module);

        FLog::Info() << "Loaded module '" << FLog::FGBlue << module_name << FLog::ResetFormatting << "'." << FLog::Flush;
    }

    FModuleIdentifier FBackendManager::ParseName(const FModuleIdentifier& requested_module_name) {
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
        FLog::Warning() << "Module '" << FLog::FGBlue << requested_module_name << FLog::ResetFormatting << "' not found." << FLog::Flush;
        FLog::Info() << "Available modules:" << FLog::Flush;
        for(const auto& pair : AvailableModules) FLog::Info() << "\t" << pair.first << FLog::Flush;
        throw Exception("Unknown module '" + requested_module_name + "'");
    }

    StrVector FBackendManager::GetAvailableModules() {
        StrVector modules;
        for(const auto& pair : AvailableModules) modules.push_back(pair.first);
        return modules;
    }

    TPointer<FSlabModule> FBackendManager::GetModule(const FModuleIdentifier& module_name) {
        auto parsed_name = ParseName(module_name);

        if(!IsModuleLoaded(parsed_name)) LoadModule(parsed_name);

        return LoadedModules[parsed_name];
    }
    bool FBackendManager::IsModuleAvailable(const FModuleIdentifier &module_name) {
        return Contains(AvailableModules, module_name);
    }

    bool FBackendManager::IsModuleLoaded(const FModuleIdentifier& module_name) {
        return Contains(LoadedModules, module_name);
    }

    void FBackendManager::RegisterAvailableBackend(const FBackendIdentifier &Name, FBackendAllocator Alloc) {

        FBackendManager::AvailableBackends[Name] = std::move(Alloc);

        FLog::Note() << "Backend '" << FLog::FGBlue << Name << FLog::ResetFormatting << "' available." << FLog::Flush;
    }

    void FBackendManager::RegisterAvailableModule(const FModuleIdentifier &Name, FModuleAllocator Alloc) {
        FBackendManager::AvailableModules[Name] = std::move(Alloc);

        FLog::Note() << "Module '"  << FLog::FGBlue << Name << FLog::ResetFormatting << "' available." << FLog::Flush;
    }

    Str FBackendManager::GetBackendName() {
        return BackendName;
    }

    void FBackendManager::UnloadAllModules() {
        LoadedModules.clear();
    }


} // Core