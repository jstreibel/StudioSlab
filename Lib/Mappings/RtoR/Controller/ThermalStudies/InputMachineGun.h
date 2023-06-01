//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTMACHINEGUN_H
#define V_SHAPE_INPUTMACHINEGUN_H

#include "Mappings/RtoR/Controller/RtoRBCInterface.h"


namespace RtoR {
    class InputMachineGun  : public RtoRBCInterface {
        DoubleParameter v1 = DoubleParameter{0.5, "v1", "Left oscillon's border speed."};
        IntegerParameter n = IntegerParameter{10, "n", "Number of shooting tiny oscillons."};

    public:
        InputMachineGun();
        auto getBoundary() const -> const void * override;
    };
}


#endif //V_SHAPE_INPUTMACHINEGUN_H
