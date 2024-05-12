//
// Created by joao on 10/05/24.
//

#ifndef STUDIOSLAB_R2TORDFT_H
#define STUDIOSLAB_R2TORDFT_H

#include "Math/Function/Maps/R2toC/R2toCDiscreteFunction.h"
#include "Math/Function/Maps/R2toR/Model/R2toRDiscreteFunction.h"

namespace R2toR {

    class R2toRDFT {
    public:

        static auto DFT(const R2toR::DiscreteFunction &in) -> std::shared_ptr<R2toC::DiscreteFunction>;

    };

} // R2toR

#endif //STUDIOSLAB_R2TORDFT_H
