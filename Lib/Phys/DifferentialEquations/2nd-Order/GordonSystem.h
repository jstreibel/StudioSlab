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

    protected:
        //const GordonState &phi_0;
        //SubState &laplacian, &dV_out;
        //Base::Function dVDPhi;

    public:
        GordonSystem(const NumericParams &params) : Slab::EquationSolver(params) {

        }

        virtual EqState &applyBC(EqState &fieldStateOut, Real t, Real dt) override {
            auto &eqState = static_cast<GordonState&>(fieldStateOut);
            //if(t==0) eqState.set(phi_0);

            return eqState;
        }

        EqState &dtF(const EqState &in, EqState &out, Real t, Real dt) override {
            throw "Not implemented";
        }
    };

}

#endif //STUDIOSLAB_GORDONSYSTEM_H
