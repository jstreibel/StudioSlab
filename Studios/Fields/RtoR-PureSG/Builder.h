//
// Created by joao on 16/05/24.
//

#ifndef STUDIOSLAB_PURESG_BUILDER_H
#define STUDIOSLAB_PURESG_BUILDER_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBuilder.h"

namespace Studios::PureSG {

    using namespace Slab;
    using namespace Slab::Math;
    using namespace Slab::Models::KGRtoR;

    class Builder : public KGRtoRBuilder {

    public:
        Builder(const Str &name, const Str &generalDescription, bool doRegister=false);

    protected:
        auto buildOpenGLOutput() -> void * override;
    };

} // PureSG

#endif //STUDIOSLAB_BUILDER_H
