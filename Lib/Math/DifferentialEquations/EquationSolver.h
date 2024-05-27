#ifndef HAMILTON_H
#define HAMILTON_H

#include "Utils/Types.h"
#include "EquationState.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "BoundaryConditions.h"

namespace Slab::Math::Base {

    template<class EquationStateType>
    class EquationSolverT {
    public:
        using EqState = EquationStateType;
        using EqBoundaryCondition = Base::BoundaryConditions<EqState>;
        using EqBoundaryCondition_ptr = Pointer<EqBoundaryCondition>;
    protected:
        const NumericConfig &params;
        EqBoundaryCondition_ptr du;

    public:
        EquationSolverT(const NumericConfig &params, EqBoundaryCondition_ptr du)
        : params(params), du(du) {}
        virtual ~EquationSolverT() {}

        virtual auto NewEqState() const -> Pointer<EqState> {
            return du->newEqState();
        };

        virtual EqState& applyBC(EqState &state, Real t, Real dt);
        virtual EqState& dtF(const EqState &in, EqState &out, Real t, Real dt) = 0;

        virtual void startStep (const EqState &in, Real t, Real dt) {};
        virtual void finishStep(const EqState &in, Real t, Real dt) {};

        EqState &operator()(const EqState &in, EqState &out, Real t, Real dt) {
            return this->dtF(in, out, t, dt);
        }
    };

    template<class EqState>
    EqState &EquationSolverT<EqState>::applyBC(EqState &state, Real t, Real dt) {
        du->apply(state, t);
        return state;
    }

    class EquationSolver : public EquationSolverT<EquationState>{
    public:
        using EqBoundaryCondition = Base::BoundaryConditions<EquationState>;

        EquationSolver(const NumericConfig &params, EqBoundaryCondition_ptr du)
        : EquationSolverT(params, du) {};

    };

    DefinePointer(EquationSolver)
}

#endif // HAMILTON_H
