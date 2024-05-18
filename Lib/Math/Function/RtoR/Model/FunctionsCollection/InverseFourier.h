//
// Created by joao on 5/09/23.
//

#ifndef STUDIOSLAB_INVERSEFOURIER_H
#define STUDIOSLAB_INVERSEFOURIER_H

#include <utility>

#include "Math/Function/RtoR/Calc/DiscreteFourierTransform.h"

namespace RtoR {

    class InverseFourier : public RtoR::Function {
        DFTResult modes;
    public:
        InverseFourier() = default;
        explicit InverseFourier(DFTResult modes, Real L)
        : modes(std::move(modes)) {};

        Real operator()(Real x) const override;

        void setModes(DFTResult _modes) { modes = std::move(_modes); }
    };

} // RtoR

#endif //STUDIOSLAB_INVERSEFOURIER_H
