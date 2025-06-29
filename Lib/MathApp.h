//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_MOLDYNAPP_H
#define STUDIOSLAB_MOLDYNAPP_H

#include "AppBase.h"
#include "Math/Numerics/NumericalRecipe.h"

namespace Slab::Math {

    class MathApp : public Core::AppBase {
        Pointer<Base::NumericalRecipe> Recipe;

    public:
        MathApp(int argc, const char **argv, Pointer<Base::NumericalRecipe> SimBuilder);

        auto run() -> int override;

    };
}

#endif //STUDIOSLAB_MOLDYNAPP_H
