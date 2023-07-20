//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_SIMULATIONSAPPR2TOR_H
#define FIELDS_SIMULATIONSAPPR2TOR_H

#include "Base/App/AppBase.h"
#include "Phys/Numerics/VoidBuilder.h"



namespace R2toR {
    namespace App {

        class Simulations : public AppBase {
            Base::Simulation::VoidBuilder::Ptr builder;

        public:


            Simulations(int argc, const char **argv, Base::Simulation::VoidBuilder::Ptr bcBuilder);

            auto run() -> int override;
        };
    }
}


#endif //FIELDS_SIMULATIONSAPPR2TOR_H
