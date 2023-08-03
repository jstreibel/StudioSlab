//
// Created by joao on 7/25/22.
//

#ifndef STUDIOSLAB_INPUTRANDOMENERGYOVERDOTPHI_H
#define STUDIOSLAB_INPUTRANDOMENERGYOVERDOTPHI_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace RtoR {
    class InputRandomEnergyOverDotPhi : public Builder {
        RealParameter E = RealParameter{1. / 24, "E", "Total energy over space."};

    public:
        InputRandomEnergyOverDotPhi();

        auto getBoundary() -> void * override;
    };

}


#endif //STUDIOSLAB_INPUTRANDOMENERGYOVERDOTPHI_H
