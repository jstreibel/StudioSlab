//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H

#include "Phys/Numerics/Output/Builder.h"
#include "Phys/Numerics/Output/OutputManager.h"
#include "R2toROutputOpenGLGeneric.h"


namespace R2toR {
    namespace OutputSystem {
        class Builder : public Numerics::OutputSystem::Builder {
        public:
            Builder(Str name, Str description);

            auto build(Str outputFileName) -> OutputManager* override;

        protected:
            virtual auto buildOpenGLOutput() -> OutputOpenGL*;

        };
    }
}


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H
