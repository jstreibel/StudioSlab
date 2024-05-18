//
// Created by joao on 10/05/24.
//

#ifndef STUDIOSLAB_R2TORDFT_H
#define STUDIOSLAB_R2TORDFT_H

#include "Math/Function/R2toC/R2toCDiscreteFunction.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"

namespace R2toR {

    class R2toRDFT {
    public:

        static auto DFTReal_symmetric(const R2toR::DiscreteFunction &in) -> std::shared_ptr<R2toC::DiscreteFunction>;
        static auto DFTReal(const R2toR::DiscreteFunction &in, int sign=-1) -> std::shared_ptr<R2toC::DiscreteFunction>;

        static auto DFT(const R2toC::DiscreteFunction &in, int sign=-1) -> std::shared_ptr<R2toC::DiscreteFunction>;



        static auto DFT_inverse_symmetric(const R2toC::DiscreteFunction &in) -> std::shared_ptr<R2toC::DiscreteFunction>;



    };

} // R2toR

#endif //STUDIOSLAB_R2TORDFT_H
