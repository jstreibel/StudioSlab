//
// Created by joao on 16/05/24.
//

#include "Builder.h"
#include "../RtoR-Modes/Sim/Monitor.h"

namespace Studios:: PureSG {
    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister)
    : KGRtoRBuilder(name, generalDescription, doRegister) {}

    void *Builder::buildOpenGLOutput() {
        return new Monitor(kg_numeric_config, *static_cast<KGEnergy *>(getHamiltonian()));
    }


} // PureSG