//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_NUKLEARMODULE_H
#define STUDIOSLAB_NUKLEARMODULE_H

#include "Core/Backend/Modules/Module.h"
#include "Core/Backend/Modules/Modules.h"

#include "Core/Backend/Implementations.h"
#include "3rdParty/NuklearInclude.h"
#include "Graphics/Modules/GraphicsModule.h"

namespace Slab::Graphics {

    class NuklearModule : public GraphicsModule {
    protected:
        nk_context *nkContext;

    public:
        explicit NuklearModule(ParentSystemWindow);

        // void beginEvents() override;
        // void endEvents() override;

        static NuklearModule *BuildModule();

        nk_context *getContext();

    };

} // Core

#endif //STUDIOSLAB_NUKLEARMODULE_H
