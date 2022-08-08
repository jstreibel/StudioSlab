//
// Created by joao on 7/25/22.
//

#ifndef STUDIOSLAB_INPUTRANDOMENERGYOVERDOTPHI_H
#define STUDIOSLAB_INPUTRANDOMENERGYOVERDOTPHI_H

#include "../../Controller/RtoRBCInterface.h"

namespace RtoR {
    class InputRandomEnergyOverDotPhi : public RtoRBCInterface {
        DoubleParameter E = DoubleParameter{1./24, "E", "Total energy over space."};

    public:
        InputRandomEnergyOverDotPhi();

        auto getBoundary() const -> const void * override;
    };

}


#endif //STUDIOSLAB_INPUTRANDOMENERGYOVERDOTPHI_H
