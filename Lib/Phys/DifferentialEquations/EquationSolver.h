#ifndef HAMILTON_H
#define HAMILTON_H

#include "Common/Types.h"
#include "EquationState.h"
#include "Phys/Numerics/Program/NumericParams.h"
#include "BoundaryConditions.h"

namespace Slab {

    template<class EquationStateType>
    class EquationSolverT {
    public:
        using EqState = EquationStateType;
        using EqBoundaryCondition = Base::BoundaryConditions<EqState>;
    protected:
        const NumericParams &params;
        Base::BoundaryConditions<EqState> &du;

    public:
        EquationSolverT(const NumericParams &params, EqBoundaryCondition &du)
        : params(params), du(du) {}
        virtual ~EquationSolverT() {}

        virtual auto NewEqState() const -> EqState* {
            return du.newEqState();
        };

        virtual EqState& applyBC(EqState &state, Real t, Real dt);
        virtual EqState& dtF(const EqState &in, EqState &out, Real t, Real dt) = 0;

        virtual void startStep (Real t, Real dt) {};
        virtual void finishStep(Real t, Real dt) {};

        EqState &operator()(const EqState &in, EqState &out, Real t, Real dt) {
            return this->dtF(in, out, t, dt);
        }
    };

    template<class EqState>
    EqState &EquationSolverT<EqState>::applyBC(EqState &state, Real t, Real dt) {
        du.apply(state, t);
        return state;
    }

    class EquationSolver : public EquationSolverT<Simulation::EquationState>{
    public:
        using EqBoundaryCondition = Base::BoundaryConditions<Simulation::EquationState>;

        EquationSolver(const NumericParams &params, EqBoundaryCondition &du)
        : EquationSolverT(params, du) {};

    };
}

#endif // HAMILTON_H