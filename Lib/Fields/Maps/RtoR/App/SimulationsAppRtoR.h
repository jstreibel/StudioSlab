//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_SIMULATIONSAPPRTOR_H
#define FIELDS_SIMULATIONSAPPRTOR_H


#include <Studios/App/AppBase.h>

class SimulationsAppRtoR : public AppBase {
public:
    SimulationsAppRtoR(int argc, const char **argv);

    auto run() -> int override;

};


#endif //FIELDS_SIMULATIONSAPPRTOR_H
