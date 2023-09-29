//
// Created by joao on 22/09/23.
//

#include "NuklearModule.h"

#include "3rdParty/Nuklear/NuklearSource.h"

#include "NuklearSFMLModule.h"
#include "NuklearGLFWModule.h"

#include "Utils/Exception.h"
#include "Core/Backend/BackendManager.h"

namespace Core {
    NuklearModule::NuklearModule(Core::BackendImplementation system)
    : system(system)
    , nkContext(nullptr)
    {
        switch (system) {
            case Uninitialized:
            case Headless:
            case VTK:
                break;
            case GLFW:
            case GLUT:
            case SFML:
                BackendManager::LoadModule(ModernOpenGL);
        }
    }

    void NuklearModule::beginEvents() {
        nk_input_begin(nkContext);
    }

    void NuklearModule::endEvents() {
        nk_input_end(nkContext);
    }

    NuklearModule *NuklearModule::BuildModule(BackendImplementation backendImplementation) {
        switch (backendImplementation) {
            case Uninitialized:
            case Headless:
            case GLUT:
            case VTK:
                NOT_IMPLEMENTED
            case GLFW:
                return new NuklearGLFWModule();
            case SFML:
                return new NuklearSFMLModule();
        }

        NOT_IMPLEMENTED
    }

    nk_context *NuklearModule::getContext() { return nkContext; };

} // Core