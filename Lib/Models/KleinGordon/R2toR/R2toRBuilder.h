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

namespace Slab::Models::KGR2toR {

    using namespace Slab::Math;

    class Builder : public Models::KGRecipe {
        Str name;

    protected:
        virtual auto buildOpenGLOutput() -> OutputOpenGL*;

    public:
        Builder(const Str& name, const Str& description, bool do_register=false);
        virtual ~Builder() = default;

        auto BuildOutputSockets()   -> Vector<TPointer<Socket>> override;
        auto buildSolver()  -> TPointer<Base::LinearStepSolver> override;

        auto newFunctionArbitrary() -> R2toR::FNumericFunction_ptr ;
        auto newFieldState()        -> R2toR::EquationState_ptr;

        auto getInitialState()      -> R2toR::EquationState_ptr;

        virtual auto getBoundary() -> Base::BoundaryConditions_ptr = 0;


    };
}


#endif //STUDIOSLAB_R2TORBUILDER_H
