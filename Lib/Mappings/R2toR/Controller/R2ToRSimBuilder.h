//
// Created by joao on 7/19/23.
//

#ifndef STUDIOSLAB_R2TORSIMBUILDER_H
#define STUDIOSLAB_R2TORSIMBUILDER_H

#include "Phys/Numerics/Builder.h"
#include "Mappings/R2toR/View/R2toROutputOpenGLGeneric.h"

#include "Mappings/R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"
#include "Mappings/R2toR/Model/EquationSolver.h"
#include "Mappings/R2toR/Model/EquationState.h"

namespace R2toR::Simulation {
    class Builder : public BuilderMap(R2toR) {
        Str name = "";

    protected:
        virtual auto buildOpenGLOutput() -> R2toR::OutputOpenGL*;

    public:
        Builder(Str name, Str description);

        auto buildOutputManager()       -> OutputManager * override;

        auto newFunctionArbitrary()     -> R2toR::DiscreteFunction * override;
        auto newFieldState()            -> R2toR::EquationState    * override;
        auto getEquationSolver()        -> R2toR::EquationSolver   * override;
    };
}


#endif //STUDIOSLAB_R2TORSIMBUILDER_H
