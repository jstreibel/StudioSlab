//
// Created by joao on 10/7/24.
//

#ifndef STUDIOSLAB_DIFF_H
#define STUDIOSLAB_DIFF_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class Diff : public RtoR::Function {
        Pointer<const RtoR::Function> base_function;
        Real dx;
    public:

        explicit Diff(Pointer<const RtoR::Function> base_function, Real dx=1.e-3);

        Real operator()(Real x) const override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_DIFF_H
