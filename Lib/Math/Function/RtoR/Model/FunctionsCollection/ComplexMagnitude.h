//
// Created by joao on 11/09/23.
//

#ifndef STUDIOSLAB_COMPLEXMAGNITUDE_H
#define STUDIOSLAB_COMPLEXMAGNITUDE_H

#include "Math/Function/RtoC/Function.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace RtoR {

    class ComplexMagnitude : public RtoR::Function {
        RtoC::Function::Ptr RtoCFunction;
    public:
        explicit ComplexMagnitude(RtoC::Function::Ptr func);

        Real operator()(Real x) const override;

    };

} // RtoR

#endif //STUDIOSLAB_COMPLEXMAGNITUDE_H
