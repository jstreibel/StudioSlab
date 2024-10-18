//
// Created by joao on 04/06/23.
//

#include "SlabCore.h"

#include "Core/Backend/Console/ConsoleBackend.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

namespace Slab::Core {

    void RegisterBackends();
    void RegisterModules();

    Backend &GetBackend() {
        return BackendManager::GetBackend();
    }

    void Register() {
        RegisterBackends();
        RegisterModules();

    }

    void Finish() {
        Core::BackendManager::UnloadAllModules();
    }

    void LoadModule(const ModuleName& module) {
        BackendManager::LoadModule(module);
    }

    Pointer<Module> GetModule(const ModuleName &module) {
        return BackendManager::GetModule(module);
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