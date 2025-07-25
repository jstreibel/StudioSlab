//
// Created by joao on 04/06/23.
//

#include "SlabCore.h"

#include "Core/Backend/Console/ConsoleBackend.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"
#include "StudioSlab.h"
#include "Core/Controller/CommandLine/CommandLineArgsManager.h"

namespace Slab::Core {

    void RegisterBackends();
    void RegisterModules();

    void StartBackend(const FBackendIdentifier &name) {
        BackendManager::Startup(name);
    }

    TPointer<FBackend> GetBackend() {
        return BackendManager::GetBackend();
    }

    void Startup() {
        RegisterBackends();
        RegisterModules();
    }

    void Finish() {
        Core::BackendManager::UnloadAllModules();
    }

    void LoadModule(const FModuleIdentifier &name) {
        GetModule(name);
    }
    TPointer<SlabModule> GetModule(const FModuleIdentifier &module) {
        if(!Slab::IsStarted()) Slab::Startup();
        return BackendManager::GetModule(module);
    }

    void RegisterCLInterface(const TPointer<FCommandLineInterface>& interface) {
        FCommandLineInterfaceManager::getInstance().registerInterface(interface);
    }

    void ParseCLArgs(int argc, const char **argv) {
        Core::CLArgsManager::Parse(argc, argv);
    }

    void RegisterBackends(){
        BackendManager::RegisterAvailableBackend("Headless", [](){
            return std::make_unique<FConsoleBackend>();
        });
    }

    void RegisterModules(){
        BackendManager::RegisterAvailableModule<MTaskManager>("TaskManager");
    }

}