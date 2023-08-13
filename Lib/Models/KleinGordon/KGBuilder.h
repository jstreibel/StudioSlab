//
// Created by joao on 8/2/23.
//

#ifndef STUDIOSLAB_KGBUILDER_H
#define STUDIOSLAB_KGBUILDER_H

#include "Phys/Numerics/VoidBuilder.h"

namespace Fields::KleinGordon {

    class KGBuilder : public Base::Simulation::VoidBuilder {
    public:
        KGBuilder(Str name="Klein-Gordon",
                  Str generalDescription="The Klein-Gordon scalar field equation builder",
                  bool doRegister=false)
                  : Base::Simulation::VoidBuilder(name, generalDescription, false) {};

        virtual void* getHamiltonian() = 0;
    };

}

#endif //STUDIOSLAB_KGBUILDER_H
