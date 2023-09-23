//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_NUKLEARMODULE_H
#define STUDIOSLAB_NUKLEARMODULE_H

#include "Module.h"
#include "Core/Controller/Nuklear/NuklearSFML.h"
#include "Modules.h"
#include "Core/Backend/Implementations.h"

namespace Core {

    class NuklearModule : public Module {
        nk_context nkContext{};

    public:
        explicit NuklearModule(Core::BackendImplementation system);

        void beginRender() override;

        void endRender() override;

        void beginEvents() override;

        void endEvents() override;

    };

} // Core

#endif //STUDIOSLAB_NUKLEARMODULE_H
