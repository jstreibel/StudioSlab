//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLEINPUT_H
#define STUDIOSLAB_GROWINGHOLEINPUT_H

#include "Fields/Mappings/R2toR/Controller/R2ToRBCInterface.h"

#include "../OutputBuilder.h"
#include "GrowingHoleBoundaryCondition.h"

const auto pi = 3.1415926535897932384626;

namespace R2toR {
    class InputGrowingHole : public R2toRBCInterface {
        DoubleParameter height=DoubleParameter{0.1, "height,h", "Discontinuity value."};

    public:
        InputGrowingHole() : R2toRBCInterface("(2+1)-d Shockwave as a growing hole.", "gh",
                                              new OutputBuilder)
        {
            addParameters({&height});
        }


        auto getBoundary() const -> const void * override{
            const double a = *this->height; // Eh isso mesmo? Não era h = 2a/3??

            return new GrowingHoleBoundaryCondition(a);
        }
    };
}


#endif //STUDIOSLAB_GROWINGHOLEINPUT_H
