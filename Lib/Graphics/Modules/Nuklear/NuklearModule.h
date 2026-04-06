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

    class FNuklearModule : public FGUIModule {
    public:
        explicit FNuklearModule();

        // void beginEvents() override;
        // void endEvents() override;

        static FNuklearModule* BuildModule();

        auto CreateContext(FOwnerPlatformWindow window) -> TPointer<FGUIContext> override;

    };

    using NuklearModule [[deprecated("Use FNuklearModule")]] = FNuklearModule;

} // Core

#endif //STUDIOSLAB_NUKLEARMODULE_H
