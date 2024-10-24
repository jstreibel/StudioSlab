//
// Created by joao on 11/09/23.
//

#ifndef STUDIOSLAB_COMPLEXMAGNITUDE_H
#define STUDIOSLAB_COMPLEXMAGNITUDE_H

#include "Math/Function/RtoC/Function.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class ComplexMagnitude : public RtoR::Function {
        RtoC::Function_ptr RtoCFunction;
    public:
        explicit ComplexMagnitude(RtoC::Function_ptr func);

        Real operator()(Real x) const override;

    };

} // RtoR

#endif //STUDIOSLAB_COMPLEXMAGNITUDE_H
