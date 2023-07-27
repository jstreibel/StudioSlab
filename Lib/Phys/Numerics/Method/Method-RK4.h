/*
 * stepper-rk.h
 *
 *  Created on: 16 de set de 2017
 *      Author: johnny
 */

#ifndef STEPPER_RK_H
#define STEPPER_RK_H

#include "Method.h"

#include "Phys/DifferentialEquations/BoundaryConditions.h"
#include "Phys/DifferentialEquations/EquationSolver.h"
#include "Phys/Numerics/VoidBuilder.h"

#include <cstring> // contains memcpy
#include <omp.h>

template<int NUM_THREADS, class STATE_TYPE>
class StepperRK4 : public Method {
public:
    typedef Slab::EquationSolverT<STATE_TYPE>    SolverType;
    typedef Base::BoundaryConditions<STATE_TYPE> BCType;

    StepperRK4(SolverType &solver, const BCType &boundaryCond)
    : Method()
    , H       (solver)
    , dPhi    (boundaryCond)
    , _phi    ( solver.NewEqState() )
    , _k1     ( solver.NewEqState() )
    , _k2     ( solver.NewEqState() )
    , _k3     ( solver.NewEqState() )
    , _k4     ( solver.NewEqState() )
    , _phiTemp( solver.NewEqState() )
    { }

    ~StepperRK4(){
        delete &H;

        delete _phi;
        delete _k1;
        delete _k2;
        delete _k3;
        delete _k4;
        delete _phiTemp;
    }

    void step(const Real &dt, const size_t n_steps) override {
        const Real dt2=0.5*dt;
        const Real dt6 = dt/6.0; (void)dt6;
        const Real inv6 = 1.0/6.0;

        STATE_TYPE &phi = *_phi;
        STATE_TYPE &k1 = *_k1;
        STATE_TYPE &k2 = *_k2;
        STATE_TYPE &k3 = *_k3;
        STATE_TYPE &k4 = *_k4;
        STATE_TYPE &phiTemp = *_phiTemp;

        for(size_t i=0; i<n_steps; ++i)
        {
            const Real t = (steps+i)*dt;
            H.startStep(t, dt);
            #pragma omp parallel num_threads(NUM_THREADS)
            {
                dPhi->apply(phi, t);

                H(phi, k1, t, dt2);
                #pragma omp barrier

                phiTemp.StoreAddition(phi, k1);
                #pragma omp barrier

                H(phiTemp, k2, t, dt2);
                #pragma omp barrier

                phiTemp.StoreAddition(phi, k2);
                #pragma omp barrier

                H(phiTemp, k3, t, dt);
                #pragma omp barrier

                phiTemp.StoreAddition(phi, k3);
                #pragma omp barrier

                H(phiTemp, k4, t, dt);
                #pragma omp barrier

                ((k2*=2.0)+=k3)*=2.0;
                (k1*=2.0)+=k4;
                phiTemp.StoreAddition(k1, k2)*=inv6;
                phi+=phiTemp;
                #pragma omp barrier
            }

            H.finishStep(t, dt);
        }
        steps+=n_steps;
    }

    [[nodiscard]] const void* getFieldState() const override { return _phi; }
    [[nodiscard]] DiscreteSpacePair getSpaces() const override {
        return DiscreteSpacePair(&_phi->getPhi().getSpace(), &_phi->getDPhiDt().getSpace());
    }

private:
    Slab::EquationSolverT<STATE_TYPE> &H;

    const Base::BoundaryConditions<STATE_TYPE> *dPhi;

    STATE_TYPE *_phi;
    STATE_TYPE *_k1, *_k2, *_k3, *_k4;
    STATE_TYPE *_phiTemp;

    int steps = 0;

};


#endif /* STEPPER_NR_RK_H_ */
