//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_APP_H
#define STUDIOSLAB_APP_H

#include "Base/App/AppBase.h"
#include "Phys/Numerics/VoidBuilder.h"

namespace Simulation {
    class App : public AppBase {
        Base::Simulation::VoidBuilder::Ptr builder;

    public:
        App(int argc, const char **argv, Base::Simulation::VoidBuilder::Ptr simBuilder);

        auto run() -> int override;

    };
}

#endif //STUDIOSLAB_APP_H
