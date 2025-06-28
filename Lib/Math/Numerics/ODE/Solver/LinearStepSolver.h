#ifndef HAMILTON_H
#define HAMILTON_H

#include <utility>

#include "Utils/Types.h"
#include "EquationState.h"
#include "BoundaryConditions.h"

namespace Slab::Math::Base {

    class LinearStepSolver {
    protected:
        Pointer<BoundaryConditions> du;

    public:
        explicit LinearStepSolver(Pointer<BoundaryConditions> du)
        : du(std::move(du)) {}

        virtual ~LinearStepSolver() = default;

        [[nodiscard]] virtual auto NewEqState() const -> EquationState_ptr;

        virtual EquationState& applyBC(EquationState &state, DevFloat t, DevFloat dt);
        virtual EquationState& F(const EquationState &in, EquationState &out, DevFloat t) = 0;

        virtual void startStep (const EquationState &in, DevFloat t, DevFloat dt) {};
        virtual void finishStep(const EquationState &in, DevFloat t, DevFloat dt) {};

        EquationState &operator()(const EquationState &in, EquationState &out, DevFloat t);
    };
}

#endif // HAMILTON_H
