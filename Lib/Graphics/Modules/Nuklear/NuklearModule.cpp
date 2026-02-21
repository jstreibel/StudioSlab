//
// Created by joao on 22/09/23.
//

#include "NuklearModule.h"

#include "3rdParty/NuklearSource.h"

#include "NuklearSFMLModule.h"
#include "NuklearGLFWModule.h"

#include "Core/Backend/BackendManager.h"
#include "Core/SlabCore.h"

// https://immediate-mode-ui.github.io/Nuklear/doc/index.html

namespace Slab::Graphics {
    FNuklearModule::FNuklearModule()
    : FGUIModule("Nuklear GUI")
    {
        fix non_opengl = Vector<Str>{"Uninitialized", "VTK", "Headless"};
        fix opengl = Vector<Str>{"GLFW", "GLUT", "SFML"};

        fix backend_name = Core::BackendManager::GetBackendName();

        if(Contains(non_opengl, backend_name))  NOT_IMPLEMENTED
        if(Contains(opengl, backend_name)) Core::LoadModule("ModernOpenGL");
    }

    // void FNuklearModule::beginEvents() {
    //     nk_input_begin(nkContext);
    // }
//
    // void FNuklearModule::endEvents() {
    //     nk_input_end(nkContext);
    // }

    FNuklearModule* FNuklearModule::BuildModule() {
        auto not_implemented = Vector<Str>{"Uninitialized", "VTK", "Headless", "GLUT"};

        Str backendImpl = Core::BackendManager::GetBackendName();
        if(Contains(not_implemented, backendImpl)) NOT_IMPLEMENTED

        if(backendImpl == "GLFW") return new FNuklearGLFWModule();
        if(backendImpl == "SFML") return new FNuklearSFMLModule();

        throw Exception("Unknown module " + backendImpl + " @ " + __PRETTY_FUNCTION__);
    }

    TPointer<FGUIContext> FNuklearModule::CreateContext(FOwnerPlatformWindow window) {
        NOT_IMPLEMENTED_CLASS_METHOD
        return Slab::TPointer<FGUIContext>();
    }

} // Core
