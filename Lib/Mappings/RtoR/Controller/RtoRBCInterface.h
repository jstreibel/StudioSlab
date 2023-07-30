//
// Created by joao on 10/18/21.
//

#ifndef FIELDS_RTORBCINTERFACE_H
#define FIELDS_RTORBCINTERFACE_H

#include "Phys/Numerics/VoidBuilder.h"
#include "Mappings/RtoR/View/Graphics/RtoRMonitor.h"

class RtoRBCInterface : public Base::Simulation::VoidBuilder {
protected:
    virtual auto buildOpenGLOutput() -> RtoR::Monitor*;

public:
    explicit RtoRBCInterface(Str name,
                             Str generalDescription);

    auto buildOutputManager()   -> OutputManager * override;

    auto newFunctionArbitrary() -> void * override;
    auto newFieldState()        -> void * override;
    auto getEquationSolver()    -> void * override;

    auto getInitialState()      -> void * override;

    auto buildStepper()         -> Method * override;
};


#endif //FIELDS_RTORBCINTERFACE_H
