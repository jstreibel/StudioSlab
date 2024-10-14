//
// Created by joao on 04/06/23.
//

#include "Base.h"

#include "Core/Backend/Console/ConsoleBackend.h"
#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Backend/SFML/SFMLBackend.h"
#include "Core/Backend/Modules/ImGui/ImGuiModule.h"
#include "Core/Backend/Modules/Animator/RealTimeAnimation.h"
#include "Core/Backend/Modules/Nuklear/NuklearModule.h"
#include "Core/Backend/Modules/ModernOpenGLModule.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

namespace Slab::Core {

    void RegisterBackends();
    void RegisterModules();

    Backend &GetBackend() {
        return BackendManager::GetBackend();
    }

    GraphicBackend &GetGUIBackend() {
        return BackendManager::GetGUIBackend();
    }

    void Register() {
        RegisterBackends();
        RegisterModules();

    }

    void RegisterBackends(){
        BackendManager::RegisterAvailableBackend<ConsoleBackend>("Headless");
        BackendManager::RegisterAvailableBackend<GLFWBackend>("GLFW");
        BackendManager::RegisterAvailableBackend<GLUTBackend>("GLUT");
        BackendManager::RegisterAvailableBackend<SFMLBackend>("SFML");
        BackendManager::RegisterAvailableBackend<SFMLBackend>("Default");
        BackendManager::RegisterAvailableBackend("VTK", [] () { throw Exception("VTKBackend not implemented"); return nullptr; }) ;
    };
    void RegisterModules(){
        BackendManager::RegisterAvailableModule("ImGui", []() { return ImGuiModule::BuildModule(); });
        BackendManager::RegisterAvailableModule("Nuklear", [](){ return NuklearModule::BuildModule(); });

        BackendManager::RegisterAvailableModule<RealTimeAnimationModule>("RealTimeAnimation");

        BackendManager::RegisterAvailableModule<ModernOpenGLModule>("ModernOpenGL");
        BackendManager::RegisterAvailableModule<TaskManagerModule>("TaskManager");
        BackendManager::RegisterAvailableModule("NanoGUI", []() {throw Exception("NanoGUI module not implemented"); return nullptr; });
        BackendManager::RegisterAvailableModule("Jack", []() {throw Exception("Jack module not implemented"); return nullptr; });
        BackendManager::RegisterAvailableModule("NodeJS", []() {throw Exception("NodeJS module not implemented"); return nullptr; });
    };

}