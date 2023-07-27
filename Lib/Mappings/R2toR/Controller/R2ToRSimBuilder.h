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

namespace R2toR {
    class Builder : public Base::Simulation::VoidBuilder {
            Str name = "";

        protected:
            virtual auto buildOpenGLOutput() -> R2toR::OutputOpenGL*;

        public:
            Builder(Str name, Str description);

            auto buildOutputManager()   -> OutputManager * override;

            auto newFunctionArbitrary() -> void * override;
            auto newFieldState()        -> void * override;
            auto getEquationSolver()    -> void * override;

        auto getInitialState() -> void * override;

        auto buildStepper() -> Method * override;
    };
}


#endif //STUDIOSLAB_R2TORSIMBUILDER_H
