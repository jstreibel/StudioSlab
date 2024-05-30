#ifndef HAMILTON_H
#define HAMILTON_H

#include <utility>

#include "Utils/Types.h"
#include "EquationState.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "BoundaryConditions.h"

namespace Slab::Math::Base {

    class Solver {
    protected:
        const NumericConfig &params;
        BoundaryConditions_ptr du;

    public:
        Solver(const NumericConfig &params, BoundaryConditions_ptr du)
        : params(params), du(std::move(du)) {}

        virtual ~Solver() = default;

        virtual auto NewEqState() const -> EquationState_ptr;

        virtual EquationState& applyBC(EquationState &state, Real t, Real dt);
        virtual EquationState& dtF(const EquationState &in, EquationState &out, Real t) = 0;

        virtual void startStep (const EquationState &in, Real t, Real dt) {};
        virtual void finishStep(const EquationState &in, Real t, Real dt) {};

        EquationState &operator()(const EquationState &in, EquationState &out, Real t);
    };

    DefinePointer(Solver)
}

#endif // HAMILTON_H