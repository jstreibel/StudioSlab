//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_ENERGY_H
#define STUDIOSLAB_ENERGY_H

#include "Math/FunctionOperations/Functional.h"
#include "Models/KleinGordon/R2toR/EquationState.h"


namespace Slab::Math::R2toR {

    namespace KleinGordon {
        typedef Functional<const R2toR::EquationState> Functional;

        class Energy : public Functional {

        public:
            DevFloat computeRadial_method1(const R2toR::EquationState &function, DevFloat upToRadius);
            DevFloat computeRadial_method2(const R2toR::EquationState &function, DevFloat upToRadius);

            DevFloat operator[](const R2toR::EquationState &function) const override;
        };

    }

}

#endif //STUDIOSLAB_ENERGY_H
