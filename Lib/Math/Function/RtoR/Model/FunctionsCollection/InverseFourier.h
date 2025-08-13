//
// Created by joao on 5/09/23.
//

#ifndef STUDIOSLAB_INVERSEFOURIER_H
#define STUDIOSLAB_INVERSEFOURIER_H

#include <utility>

#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"

namespace Slab::Math::RtoR {

    class InverseFourier : public RtoR::Function {
        DFTResult modes;
    public:
        InverseFourier() = default;
        explicit InverseFourier(DFTResult modes, DevFloat L)
        : modes(std::move(modes)) {};

        DevFloat operator()(DevFloat x) const override;

        void setModes(DFTResult _modes) { modes = std::move(_modes); }
    };

} // RtoR

#endif //STUDIOSLAB_INVERSEFOURIER_H
