//
// Created by joao on 7/21/23.
//

#ifndef STUDIOSLAB_GORDONSYSTEM_H
#define STUDIOSLAB_GORDONSYSTEM_H

#include "Phys/DifferentialEquations/EquationSolver.h"
#include "GordonState.h"

namespace Phys::Gordon {

    class GordonSystem : public Base::EquationSolver {
        typedef Simulation::EquationState SubState;
    protected:
        const GordonState &phi_0;
        SubState &laplacian, &dV_out;
        Base::Function dVDPhi;

    public:
        GordonSystem(const NumericParams &params) : Base::EquationSolver(params) {

        }

        virtual EqState &applyBC(EqState &fieldStateOut, Real t, Real dt) {
            if(t==0) fieldStateOut.set(phi_0);

            return fieldStateOut;
        }

        EqState &dtF(const EqState &in, EqState &out, Real t, Real dt) override {
            return <#initializer#>;
        }
    };

}

#endif //STUDIOSLAB_GORDONSYSTEM_H
