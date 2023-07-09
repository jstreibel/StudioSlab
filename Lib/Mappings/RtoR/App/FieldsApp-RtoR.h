//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_SIMULATIONSAPPRTOR_H
#define FIELDS_SIMULATIONSAPPRTOR_H


#include "Base/App/AppBase.h"
#include "Mappings/SimulationBuilder.h"

class SimulationsAppRtoR : public AppBase {
    Base::SimulationBuilder::Ptr simBuilder;
public:
    enum Integration {regular, langevin, montecarlo} integration;

    SimulationsAppRtoR(int argc, const char **argv, Base::SimulationBuilder::Ptr simBuilder);

    auto run() -> int override;

};


#endif //FIELDS_SIMULATIONSAPPRTOR_H
