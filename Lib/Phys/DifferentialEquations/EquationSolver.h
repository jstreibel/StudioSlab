#ifndef HAMILTON_H
#define HAMILTON_H

#include "Common/Types.h"
#include "EquationState.h"
#include "Phys/Numerics/Program/NumericParams.h"

namespace Slab {

    template<class EquationStateType>
    class EquationSolverT {
    public:
        using EqState = EquationStateType;

    protected:
        const NumericParams &params;
        const EqState &u_0;

    public:
        EquationSolverT(const NumericParams &params, const EqState &u_0) : params(params), u_0(u_0) {}
        virtual ~EquationSolverT() {}

        virtual auto NewEqState() const -> EqState* = 0;

        virtual EqState& applyBC(EqState &fieldStateOut, Real t, Real dt) = 0;
        virtual EqState& dtF(const EqState &in, EqState &out, Real t, Real dt) = 0;

        virtual void startStep (Real t, Real dt) {};
        virtual void finishStep(Real t, Real dt) {};

        EqState &operator()(const EqState &in, EqState &out, Real t, Real dt) {
            this->applyBC(out, t, dt);
            return this->dtF(in, out, t, dt);
        }
    };

    class EquationSolver : public EquationSolverT<Simulation::EquationState>{
    public:
        EquationSolver(const NumericParams &params, const Simulation::EquationState &u_0)
        : EquationSolverT(params, u_0) {};

    };
}

#endif // HAMILTON_H
