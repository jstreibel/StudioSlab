//
// Created by joao on 16/05/24.
//

#include "Builder.h"

namespace PureSG {
    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister)
    : KGBuilder(name, generalDescription, doRegister) {}

    void *Builder::buildOpenGLOutput() {
        return KGBuilder::buildOpenGLOutput();
    }

    void *Builder::getBoundary() {
        return nullptr;
    }


} // PureSG