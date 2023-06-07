//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLEINPUT_H
#define STUDIOSLAB_GROWINGHOLEINPUT_H

#include "Mappings/R2toR/Controller/R2ToRBCInterface.h"

#include "OutputBuilder.h"
#include "GrowingHoleBoundaryCondition.h"

const auto pi = 3.1415926535897932384626;

namespace R2toR {
    namespace GrowingHole {

        class Input : public R2toRBCInterface {
            RealParameter height = RealParameter{2. / 3, "height,h", "Discontinuity value."};

        public:
            Input() : R2toRBCInterface("(2+1)-d Shockwave as a growing hole.", "gh",
                                                  new OutputBuilder) {
                addParameters({&height});
            }


            auto getBoundary() const -> const void * override {
                const Real a = *this->height; // Eh isso mesmo? Não era h = 2a/3??

                return new GrowingHoleBoundaryCondition(a);
            }
        };
    }
}


#endif //STUDIOSLAB_GROWINGHOLEINPUT_H
