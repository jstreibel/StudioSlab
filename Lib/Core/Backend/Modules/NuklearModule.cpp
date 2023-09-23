//
// Created by joao on 22/09/23.
//

#include "NuklearModule.h"
#include "Core/Controller/Nuklear/_nuklear.h"
#include "Core/Controller/Nuklear/_nuklear_sfml.hpp"

namespace Core {
    NuklearModule::NuklearModule(Core::BackendImplementation system) {
        if(system != Core::SFML) NOT_IMPLEMENTED

        nk_init_default(&nkContext, nullptr);
    }

    void NuklearModule::beginRender() {

    }

    void NuklearModule::endRender() {

    }

    void NuklearModule::beginEvents() {
        nk_input_begin(&nkContext);
    }

    void NuklearModule::endEvents() {
        nk_input_end(&nkContext);
    }

} // Core