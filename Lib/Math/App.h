//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_MOLDYNAPP_H
#define STUDIOSLAB_MOLDYNAPP_H

#include "App/AppBase.h"
#include "Math/Numerics/NumericalRecipe.h"

namespace Slab::Math {
    class App : public AppBase {
        Base::NumericalRecipe_ptr builder;

    public:
        App(int argc, const char **argv, Base::NumericalRecipe_ptr simBuilder);

        auto run() -> int override;

    };
}

#endif //STUDIOSLAB_MOLDYNAPP_H
