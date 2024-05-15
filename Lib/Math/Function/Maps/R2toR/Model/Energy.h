//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_ENERGY_H
#define STUDIOSLAB_ENERGY_H

#include "Math/Functional/Functional.h"
#include "Models/KleinGordon/R2toR/EquationState.h"


namespace Fields {

    namespace KleinGordon {
        typedef Functionals::Functional<const R2toR::EquationState> Functional;

        class Energy : public Functional {

        public:
            Real computeRadial_method1(const R2toR::EquationState &function, Real upToRadius);
            Real computeRadial_method2(const R2toR::EquationState &function, Real upToRadius);

            Real operator[](const R2toR::EquationState &function) const override;
        };

    }

}

#endif //STUDIOSLAB_ENERGY_H