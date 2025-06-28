//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_MOLDYNAPP_H
#define STUDIOSLAB_MOLDYNAPP_H

#include "AppBase.h"
#include "Math/Numerics/NumericalRecipe.h"

namespace Slab::Math {

    class MathApp : public Core::AppBase {
        Base::NumericalRecipe_ptr Builder;

    public:
        MathApp(int argc, const char **argv, Base::NumericalRecipe_ptr SimBuilder);

        auto run() -> int override;

    };
}

#endif //STUDIOSLAB_MOLDYNAPP_H
