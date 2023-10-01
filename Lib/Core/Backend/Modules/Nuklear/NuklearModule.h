//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_NUKLEARMODULE_H
#define STUDIOSLAB_NUKLEARMODULE_H

#include "../Module.h"
#include "../Modules.h"

#include "Core/Backend/Implementations.h"
#include "3rdParty/Nuklear/NuklearInclude.h"

namespace Core {

    class NuklearModule : public Module {
        Core::BackendImplementation system;
    protected:
        nk_context *nkContext;

    public:
        explicit NuklearModule(Core::BackendImplementation system);

        void beginEvents() override;
        void endEvents() override;

        static NuklearModule *BuildModule(BackendImplementation backendImplementation);

        nk_context *getContext();

    };

} // Core

#endif //STUDIOSLAB_NUKLEARMODULE_H