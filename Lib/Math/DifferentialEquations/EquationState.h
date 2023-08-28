//
// Created by joao on 7/21/23.
//

#ifndef STUDIOSLAB_EQUATIONSTATE_H
#define STUDIOSLAB_EQUATIONSTATE_H


#include "Common/Types.h"
#include "Math/Space/Impl/ArithmeticOperationInterface.h"

namespace Simulation {

    class EquationState : public Utils::ArithmeticOpsInterface<EquationState>{
    public:
        typedef std::shared_ptr<EquationState> Ptr;
        typedef std::shared_ptr<const EquationState> ConstPtr;

        virtual EquationState *clone() const = 0;

        virtual void set(const EquationState&) = 0;
    };
}


#endif //STUDIOSLAB_EQUATIONSTATE_H
