//
// Created by joao on 7/19/23.
//

#ifndef STUDIOSLAB_R2TORBUILDER_H
#define STUDIOSLAB_R2TORBUILDER_H

#include "Math/Numerics/Builder.h"
#include "Mappings/R2toR/View/R2toROutputOpenGLGeneric.h"

#include "Models/KleinGordon/R2toR/BoundaryConditions/R2ToRBoundaryCondition.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"
#include "EquationSolver.h"
#include "EquationState.h"

namespace R2toR {
    class Builder : public Core::Simulation::VoidBuilder {
            Str name = "";

        protected:
            virtual auto buildOpenGLOutput() -> R2toR::OutputOpenGL*;

        public:
            Builder(Str name, Str description);

            auto buildOutputManager()   -> OutputManager * override;

            auto newFunctionArbitrary() -> void * override;
            auto newFieldState()        -> void * override;
            auto buildEquationSolver()    -> void * override;

            auto getInitialState()      -> void * override;

            auto buildStepper()         -> Stepper * override;
    };
}


#endif //STUDIOSLAB_R2TORBUILDER_H
