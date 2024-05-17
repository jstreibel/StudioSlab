//
// Created by joao on 16/05/24.
//

#ifndef STUDIOSLAB_BUILDER_H
#define STUDIOSLAB_BUILDER_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace PureSG {

    class Builder : public RtoR::KGBuilder {

    public:
        Builder(const Str &name, const Str &generalDescription, bool doRegister=false);

    protected:
        auto buildOpenGLOutput() -> void * override;

    public:
        auto getBoundary() -> void * override;
    };

} // PureSG

#endif //STUDIOSLAB_BUILDER_H
