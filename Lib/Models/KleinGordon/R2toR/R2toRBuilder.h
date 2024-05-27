//
// Created by joao on 7/19/23.
//

#ifndef STUDIOSLAB_R2TORBUILDER_H
#define STUDIOSLAB_R2TORBUILDER_H

#include "Math/Numerics/NumericalRecipe.h"

#include "Models/KleinGordon/R2toR/BoundaryConditions/R2ToRBoundaryCondition.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"
#include "EquationSolver.h"
#include "EquationState.h"
#include "Models/KleinGordon/KGBuilder.h"
#include "Models/KleinGordon/R2toR/Graphics/R2toROutputOpenGLGeneric.h"

namespace Slab::Math::R2toR {
    class Builder : public Models::KGBuilder {
            Str name = "";

        protected:
            virtual auto buildOpenGLOutput() -> R2toR::OutputOpenGL*;

        public:
            Builder(const Str& name, Str description);

            auto buildOutputManager()   -> OutputManager * override;
            auto buildEquationSolver()  -> Base::EquationSolver_ptr override;
            auto buildStepper()         -> Stepper * override;

            auto newFunctionArbitrary() -> R2toR::DiscreteFunction_ptr ;
            auto newFieldState()        -> R2toR::EquationState_ptr;

            auto getInitialState()      -> R2toR::EquationState_ptr;

            virtual auto getBoundary() -> R2toR::BoundaryCondition_ptr = 0;


    };
}


#endif //STUDIOSLAB_R2TORBUILDER_H
