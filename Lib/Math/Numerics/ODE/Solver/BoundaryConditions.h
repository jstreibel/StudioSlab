//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_BOUNDARY_CONDITIONS_H
#define V_SHAPE_BOUNDARY_CONDITIONS_H

#include "Utils/Types.h"
#include "EquationState.h"

namespace Slab::Math::Base {

    class BoundaryConditions {

        TPointer<const EquationState> prototype;

    public:
        virtual ~BoundaryConditions() = default;

        explicit BoundaryConditions(TPointer<const EquationState> prototype);

        [[nodiscard]] EquationState_ptr NewEqState(std::optional<Str> Name) const;

        virtual void Apply(EquationState &toFunction, DevFloat t) const = 0;
    };

    DefinePointers(BoundaryConditions)
}


#endif //V_SHAPE_BOUNDARY_CONDITIONS_H
