//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_SIMULATIONSAPPRTOR_H
#define FIELDS_SIMULATIONSAPPRTOR_H


#include "Base/App/AppBase.h"
#include "Phys/Numerics/VoidBuilder.h"

class SimulationsAppRtoR : public AppBase {
    Base::Simulation::VoidBuilder::Ptr simBuilder;
public:
    SimulationsAppRtoR(int argc, const char **argv, Base::Simulation::VoidBuilder::Ptr simBuilder);

    auto run() -> int override;

};


#endif //FIELDS_SIMULATIONSAPPRTOR_H
