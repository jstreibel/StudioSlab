//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_NUKLEARMODULE_H
#define STUDIOSLAB_NUKLEARMODULE_H

#include "Core/Backend/Modules/Module.h"
#include "Core/Backend/Modules/Modules.h"

#include "Core/Backend/Implementations.h"
#include "3rdParty/NuklearInclude.h"

namespace Slab::Core {

    class NuklearModule : public Core::Module {
        Core::BackendImplementation system;
    protected:
        nk_context *nkContext;

    public:
        explicit NuklearModule(Core::BackendImplementation);

        void beginEvents() override;
        void endEvents() override;

        static NuklearModule *BuildModule(BackendImplementation);

        nk_context *getContext();

    };

} // Core

#endif //STUDIOSLAB_NUKLEARMODULE_H
