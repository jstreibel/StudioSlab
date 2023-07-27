//
// Created by joao on 7/21/23.
//

#ifndef STUDIOSLAB_GORDONSYSTEM_H
#define STUDIOSLAB_GORDONSYSTEM_H

#include "Phys/DifferentialEquations/EquationSolver.h"
#include "GordonState.h"
#include "Phys/Function/Function.h"

namespace Phys::Gordon {

    class GordonSystem : public Slab::EquationSolver {
        typedef Simulation::EquationState EqState;
        typedef Simulation::EquationState SubState;
        using BC = Slab::EquationSolver::EqBoundaryCondition;

    protected:
        //const GordonState &phi_0;
        //SubState &laplacian, &dV_out;
        //Base::Function dVDPhi;

    public:
        GordonSystem(const NumericParams &params, BC &du) : Slab::EquationSolver(params, du) {

        }

        virtual EqState &applyBC(EqState &u, Real t, Real dt) override {
            du.apply(u, t);

            return u;
        }

        EqState &dtF(const EqState &in, EqState &out, Real t, Real dt) override {
            throw "Not implemented";
        }
    };

}

#endif //STUDIOSLAB_GORDONSYSTEM_H
