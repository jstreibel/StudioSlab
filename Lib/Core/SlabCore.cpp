//
// Created by joao on 04/06/23.
//

#include "SlabCore.h"

#include "Core/Backend/Console/ConsoleBackend.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "StudioSlab.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"

namespace Slab::Core {

    void RegisterBackends();
    void RegisterModules();

    void StartBackend(const BackendName &name) {
        BackendManager::Startup(name);
    }

    Backend &GetBackend() {
        return BackendManager::GetBackend();
    }

    void Startup() {
        RegisterBackends();
        RegisterModules();
    }

    void Finish() {
        Core::BackendManager::UnloadAllModules();
    }

    void LoadModule(const ModuleName& module) {
        if(!Slab::IsStarted()) Slab::Startup();
        BackendManager::LoadModule(module);
    }

    Pointer<Module> GetModule(const ModuleName &module) {
        return BackendManager::GetModule(module);
    }

    void RegisterCLInterface(const Pointer<CLInterface>& interface) {
        CLInterfaceManager::getInstance().registerInterface(interface);
    }

    void ParseCLArgs(int argc, const char **argv) {
        Core::CLArgsManager::Parse(argc, argv);
    }

    void RegisterBackends(){
        BackendManager::RegisterAvailableBackend("Headless", [](){
            return std::make_unique<ConsoleBackend>();
        });
    };
    void RegisterModules(){
        BackendManager::RegisterAvailableModule<TaskManagerModule>("TaskManager");
    };

}