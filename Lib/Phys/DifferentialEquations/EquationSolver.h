#ifndef HAMILTON_H
#define HAMILTON_H

#include "Common/Types.h"
#include "EquationState.h"
#include "Phys/Numerics/Program/NumericParams.h"

namespace Slab {

    template<typename EquationStateType>
    class EquationSolverT {
        using EqState = EquationStateType;

    protected:
        const NumericParams &params;

    public:
        EquationSolverT(const NumericParams &params) : params(params) {}
        virtual ~EquationSolverT() {}

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
        EquationSolver(const NumericParams &params) : EquationSolverT(params) {};

    };
}

#endif // HAMILTON_H
