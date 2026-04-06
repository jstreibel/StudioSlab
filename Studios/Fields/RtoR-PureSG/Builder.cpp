//
// Created by joao on 16/05/24.
//

#include "Builder.h"
#include "../RtoR-Modes/Sim/Monitor.h"

namespace Studios::PureSG {
    FBuilder::FBuilder(const Str &name, const Str &generalDescription, bool doRegister)
    : FKGRtoR_Recipe(name, generalDescription, doRegister) {}

    void *FBuilder::BuildOpenGLOutput() {
        return new Monitor(KGNumericConfig, *static_cast<FKGEnergy *>(getHamiltonian()));
    }


} // PureSG
