//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_METROPOLISR2TORRECIPE_H
#define STUDIOSLAB_METROPOLISR2TORRECIPE_H

#include "Math/Numerics/NumericalRecipe.h"

namespace Slab::Math {

    class MetropolisR2toRRecipe : public Base::NumericalRecipe {
    public:
        auto buildOutputSockets() -> Vector<Pointer<Socket>> override;

        auto buildStepper() -> Pointer<Stepper> override;
    };

} // Slab::Math

#endif //STUDIOSLAB_METROPOLISR2TORRECIPE_H
