//
// Created by joao on 7/21/23.
//

#ifndef STUDIOSLAB_EQUATIONSTATE_H
#define STUDIOSLAB_EQUATIONSTATE_H


#include "Utils/Types.h"
#include "Math/VectorSpace/Algebra/NumericAlgebra.h"

namespace Slab::Math::Base {

    class EquationState;
    using StateAlgebra = NumericAlgebra<EquationState>;

    class EquationState : public StateAlgebra {
    public:
        // using EquationAlgebra::operator-;
        // using EquationAlgebra::operator+;
        using StateAlgebra::operator=;

        /**
         * Produce a replica of this EquationState with all parameters replicated, except for the state data itself.
         * @return the replica.
         */
        virtual auto replicate() const -> Pointer<EquationState> = 0;

        /**
         * Sets this EquationState's data to match that of other state. Notice that both state's parameters (dimensions,
         * discretization, etc.) should be equal, as well as (of course) their type.
         */
        virtual void setData(const EquationState&) = 0;
    };

    DefinePointers(EquationState)
}

#endif //STUDIOSLAB_EQUATIONSTATE_H
