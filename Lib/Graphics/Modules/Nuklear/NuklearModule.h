//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_NUKLEARMODULE_H
#define STUDIOSLAB_NUKLEARMODULE_H

#include "Core/Backend/Modules/Module.h"
#include "Core/Backend/Modules/Modules.h"

#include "Core/Backend/Implementations.h"
#include "3rdParty/NuklearInclude.h"
#include "Graphics/Modules/GUIModule/GUIModule.h"

namespace Slab::Graphics {

    class NuklearModule : public FGUIModule {
    public:
        explicit NuklearModule();

        // void beginEvents() override;
        // void endEvents() override;

        static NuklearModule *BuildModule();

        auto CreateContext(FOwnerPlatformWindow window) -> Pointer<GUIContext> override;

    };

} // Core

#endif //STUDIOSLAB_NUKLEARMODULE_H
