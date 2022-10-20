//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLEINPUT_H
#define STUDIOSLAB_GROWINGHOLEINPUT_H

#include <Fields/Mappings/R2toR/Controller/R2ToRBCInterface.h>

namespace R2toR {
    class InputGrowingHole : public R2toRBCInterface {
        DoubleParameter height=DoubleParameter{0.1, "height,h", "Discontinuity value."};

    public:
        InputGrowingHole();

        auto getBoundary() const -> const void * override;
    };
}


#endif //STUDIOSLAB_GROWINGHOLEINPUT_H
