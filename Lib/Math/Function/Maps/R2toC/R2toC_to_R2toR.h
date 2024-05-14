//
// Created by joao on 13/05/24.
//

#ifndef STUDIOSLAB_R2TOC_TO_R2TOR_H
#define STUDIOSLAB_R2TOC_TO_R2TOR_H

#include "Math/Function/Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "R2toCDiscreteFunction.h"

namespace Math {
    enum R2toC_to_R2toR_Mode {
        Phase,
        Magnitude,
        Real,
        Imaginary
    };

    std::shared_ptr<R2toR::DiscreteFunction> Convert(const std::shared_ptr<const R2toC::DiscreteFunction>&, R2toC_to_R2toR_Mode);

    std::shared_ptr<R2toR::DiscreteFunction> ConvertToAbs       (const std::shared_ptr<const R2toC::DiscreteFunction>& in,
                                                                 std::shared_ptr<R2toR::DiscreteFunction> out);
    std::shared_ptr<R2toR::DiscreteFunction> ConvertToPhase     (const std::shared_ptr<const R2toC::DiscreteFunction>& in,
                                                                 std::shared_ptr<R2toR::DiscreteFunction> out);
    std::shared_ptr<R2toR::DiscreteFunction> ConvertToReal      (const std::shared_ptr<const R2toC::DiscreteFunction> &in,
                                                                 std::shared_ptr<R2toR::DiscreteFunction> out);
    std::shared_ptr<R2toR::DiscreteFunction> ConvertToImaginary (const std::shared_ptr<const R2toC::DiscreteFunction> &in,
                                                                 std::shared_ptr<R2toR::DiscreteFunction> out);
}

#endif //STUDIOSLAB_R2TOC_TO_R2TOR_H
