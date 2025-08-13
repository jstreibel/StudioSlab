//
// Created by joao on 10/7/24.
//

#ifndef STUDIOSLAB_DIFF_H
#define STUDIOSLAB_DIFF_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class Diff : public RtoR::Function {
        TPointer<const RtoR::Function> base_function;
        DevFloat dx;
    public:

        explicit Diff(TPointer<const RtoR::Function> base_function, DevFloat dx=1.e-3);

        DevFloat operator()(DevFloat x) const override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_DIFF_H
