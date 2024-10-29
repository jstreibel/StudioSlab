//
// Created by joao on 7/19/23.
//

#ifndef STUDIOSLAB_R2TORBUILDER_H
#define STUDIOSLAB_R2TORBUILDER_H

#include "Models/KleinGordon/R2toR/BoundaryConditions/R2ToRBoundaryCondition.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "EquationState.h"
#include "Models/KleinGordon/KG-Recipe.h"
#include "Models/KleinGordon/R2toR/Graphics/R2toROutputOpenGLGeneric.h"

namespace Slab::Math::R2toR {

    class Builder : public Models::KGRecipe {
            Str name = "";

        protected:
            virtual auto buildOpenGLOutput() -> R2toR::OutputOpenGL*;

        public:
            Builder(const Str& name, Str description);

            auto buildOutputManager()   -> Pointer<OutputManager> override;
            auto buildSolver()  -> Pointer<Base::LinearStepSolver> override;

            auto newFunctionArbitrary() -> R2toR::NumericFunction_ptr ;
            auto newFieldState()        -> R2toR::EquationState_ptr;

            auto getInitialState()      -> R2toR::EquationState_ptr;

            virtual auto getBoundary() -> Base::BoundaryConditions_ptr = 0;


    };
}


#endif //STUDIOSLAB_R2TORBUILDER_H
