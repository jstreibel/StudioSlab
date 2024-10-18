//
// Created by joao on 22/09/23.
//

#include "NuklearModule.h"

#include "3rdParty/NuklearSource.h"

#include "NuklearSFMLModule.h"
#include "NuklearGLFWModule.h"

#include "Core/Backend/BackendManager.h"

// https://immediate-mode-ui.github.io/Nuklear/doc/index.html

namespace Slab::Core {
    NuklearModule::NuklearModule()
    : Core::GraphicsModule("Nuklear GUI")
    , nkContext(nullptr)
    {
        fix non_opengl = Vector<Str>{"Uninitialized", "VTK", "Headless"};
        fix opengl = Vector<Str>{"GLFW", "GLUT", "SFML"};

        fix backend_name = BackendManager::GetBackendName();

        if(Contains(non_opengl, backend_name))  NOT_IMPLEMENTED;
        if(Contains(opengl, backend_name)) BackendManager::LoadModule("ModernOpenGL");
    }

    void NuklearModule::beginEvents() {
        nk_input_begin(nkContext);
    }

    void NuklearModule::endEvents() {
        nk_input_end(nkContext);
    }

    NuklearModule *NuklearModule::BuildModule() {
        auto not_implemented = Vector<Str>{"Uninitialized", "VTK", "Headless", "GLUT"};

        Str backendImpl = BackendManager::GetBackendName();
        if(Contains(not_implemented, backendImpl)) NOT_IMPLEMENTED;

        if(backendImpl == "GLFW") return new NuklearGLFWModule();
        if(backendImpl == "SFML") return new NuklearSFMLModule();

        throw Exception("Unknown module " + backendImpl + " @ " + __PRETTY_FUNCTION__);
    }

    nk_context *NuklearModule::getContext() { return nkContext; };

} // Core