//
// Created by joao on 7/21/23.
//

#ifndef STUDIOSLAB_EQUATIONSTATE_H
#define STUDIOSLAB_EQUATIONSTATE_H


#include "Utils/Types.h"
#include "Math/Space/Impl/ArithmeticOperationInterface.h"

namespace Slab::Math::Base {

    class EquationState : public ArithmeticOpsInterface<EquationState>{
    public:
        virtual Pointer<EquationState> clone() const = 0;
        virtual void set(const EquationState&) = 0;
    };

    DefinePointer(EquationState)
}


#endif //STUDIOSLAB_EQUATIONSTATE_H
