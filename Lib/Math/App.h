//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_MOLDYNAPP_H
#define STUDIOSLAB_APP_H

#include "Core/App/AppBase.h"
#include "Math/Numerics/VoidBuilder.h"

namespace Slab::Math {
    class App : public AppBase {
        VoidBuilder::Ptr builder;

    public:
        App(int argc, const char **argv, VoidBuilder::Ptr simBuilder);

        auto run() -> int override;

    };
}

#endif //STUDIOSLAB_MOLDYNAPP_H
