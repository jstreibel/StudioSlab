//
// Created by joao on 04/06/23.
//

#include "SlabCore.h"

#include "Core/Backend/Console/ConsoleBackend.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Controller/InterfaceManager.h"
#include "StudioSlab.h"
#include "Controller/CommandLine/CommandLineArgsManager.h"

namespace Slab::Core {

    void RegisterBackends();
    void RegisterModules();

    void StartBackend(const FBackendIdentifier &name) {
        FBackendManager::Startup(name);
    }

    TPointer<FBackend> GetBackend() {
        return FBackendManager::GetBackend();
    }

    void Startup() {
        RegisterBackends();
        RegisterModules();
    }

    void Finish() {
        Core::FBackendManager::UnloadAllModules();
    }

    void LoadModule(const FModuleIdentifier &name) {
        GetModule(name);
    }
    TPointer<FSlabModule> GetModule(const FModuleIdentifier &module) {
        if(!Slab::IsStarted()) Slab::Startup();
        return FBackendManager::GetModule(module);
    }

    void RegisterCLInterface(const TPointer<FInterface>& interface) {
        FInterfaceManager::GetInstance().RegisterInterface(interface);
    }

    void ParseCLArgs(int argc, const char **argv) {
        Core::FCLArgsManager::Parse(argc, argv);
    }

    void RegisterBackends(){
        FBackendManager::RegisterAvailableBackend("Headless", [](){
            return std::make_unique<FConsoleBackend>();
        });
    }

    void RegisterModules(){
        FBackendManager::RegisterAvailableModule<FTaskManager>("TaskManager");
    }

}