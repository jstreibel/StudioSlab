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
        virtual ~EquationState() = default;

        // using EquationAlgebra::operator-;
        // using EquationAlgebra::operator+;
        using StateAlgebra::operator=;

        /**
         * Produce a replica of this EquationState with all parameters replicated, except for the state data itself.
         * @return the replica.
         */
        [[nodiscard]] virtual auto Replicate(std::optional<Str> Name) const -> TPointer<EquationState> = 0;

        /**
         * Gets the (unique) category of the EquationState: first order R->R, second order, etc.
         * @return a string with the unique category of the EquationState
         */
        [[nodiscard]] virtual auto category() const -> Str = 0;

        /**
         * Sets this EquationState's data to match that of other state. Notice that both state's parameters (dimensions,
         * discretization, etc.) should be equal, as well as (of course) their type.
         */
        virtual void setData(const EquationState&) = 0;
    };

    DefinePointers(EquationState)
}

#endif //STUDIOSLAB_EQUATIONSTATE_H
