//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_MOLDYNAPP_H
#define STUDIOSLAB_APP_H

#include "Core/App/AppBase.h"
#include "Math/Numerics/VoidBuilder.h"

namespace Simulation {
    class App : public AppBase {
        Core::Simulation::VoidBuilder::Ptr builder;

    public:
        App(int argc, const char **argv, Core::Simulation::VoidBuilder::Ptr simBuilder);

        auto run() -> int override;

    };
}

#endif //STUDIOSLAB_MOLDYNAPP_H
