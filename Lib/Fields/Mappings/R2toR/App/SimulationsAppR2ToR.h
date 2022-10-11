//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_SIMULATIONSAPPR2TOR_H
#define FIELDS_SIMULATIONSAPPR2TOR_H

#include <Studios/App/AppBase.h>

class SimulationsAppR2toR : public AppBase {
public:
    SimulationsAppR2toR(int argc, const char **argv);

    auto run() -> int override;
};


#endif //FIELDS_SIMULATIONSAPPR2TOR_H
