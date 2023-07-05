//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_ENERGY_H
#define STUDIOSLAB_ENERGY_H

#include "Phys/Functional/Functional.h"
#include "Mappings/R2toR/Model/EquationState.h"


namespace Phys {

    namespace Gordon {
        typedef Functionals::Functional<const R2toR::EquationState> Functional;

        class Energy : public Functional {

        public:
            Real computeRadial(const R2toR::EquationState &function, Real upToRadius);

            Real operator[](const R2toR::EquationState &function) const override;
        };

    }

}

#endif //STUDIOSLAB_ENERGY_H
