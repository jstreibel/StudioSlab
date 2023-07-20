//
// Created by joao on 10/18/21.
//

#ifndef FIELDS_RTORBCINTERFACE_H
#define FIELDS_RTORBCINTERFACE_H

#include "Phys/Numerics/Builder.h"
#include "Mappings/RtoR/View/Graphics/RtoRMonitor.h"

class RtoRBCInterface : public Base::Simulation::Builder {
protected:
    virtual auto buildOpenGLOutput() -> RtoR::Monitor*;

public:
    explicit RtoRBCInterface(Str name,
                             Str generalDescription);

    auto buildOutputManager()   -> OutputManager * override;

    auto newFunctionArbitrary() -> void * override;

    auto newFieldState()        -> void * override;

    auto getSystemSolver()      -> void * override;


};


#endif //FIELDS_RTORBCINTERFACE_H
