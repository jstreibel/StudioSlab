//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_MOLDYNAPP_H
#define STUDIOSLAB_MOLDYNAPP_H

#include "AppBase.h"
#include "Math/Numerics/NumericalRecipe.h"

namespace Slab::Math {

    class FMathApp : public Core::FAppBase {
        TPointer<Base::FNumericalRecipe> Recipe;

    public:
        FMathApp(int argc, const char **argv, TPointer<Base::FNumericalRecipe> SimBuilder);

        auto run() -> int override;

    };

    using MathApp [[deprecated("Use FMathApp")]] = FMathApp;
}

#endif //STUDIOSLAB_MOLDYNAPP_H
