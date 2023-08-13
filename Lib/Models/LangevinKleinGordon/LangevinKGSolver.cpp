//
// Created by joao on 12/08/23.
//

#include "LangevinKGSolver.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"

Real xi() {
    const Real z = FRANDOM;

    const Real r = sqrt(-2.0 * log(1.0 - z));

    static Real signs[] = {-1,1};
    const Real sign = signs[random()%2];

    return sign*r;
}

void RtoR::LangevinKGSolver::startStep(Real t, Real dt) {
    RtoR::KGSolver ::startStep(t, dt);

    ComputeImpulses();
}

void RtoR::LangevinKGSolver::ComputeImpulses() {
    if(langevinImpulses == nullptr) return;

    auto &space = langevinImpulses->getSpace();
    assert(space.getDim().getNDim() == 1);

    auto &X = space.getHostData();

    for (auto &x: X)
        x = xi();

    space.upload();
}

void RtoR::LangevinKGSolver::setTemperature(Real value) {T = value;}

void RtoR::LangevinKGSolver::setDissipationCoefficient(Real value) {k = value;}

RtoR::EquationState &
RtoR::LangevinKGSolver::dtF(const RtoR::EquationState &stateIn, RtoR::EquationState &stateOut, Real t, Real dt) {
    if (langevinImpulses == nullptr) {
        assert(scaledImpulses == nullptr);

        langevinImpulses = (EqState::SubStateType *) stateIn.getPhi().Clone();
        scaledImpulses = (EqState::SubStateType *) stateIn.getPhi().Clone();

        scaledImpulses->Set(RtoR::NullFunction());
    }

    stateOut = RtoR::KGSolver::dtF(stateIn, stateOut, t, dt);

    {
        IN pi  = stateIn.getDPhiDt();
        OUT dissipation = *temp1;
        dissipation.StoreMultiplication(pi, -k);

        fix alpha = sqrt(2 * T / dt);
        scaledImpulses->StoreMultiplication(*langevinImpulses, alpha);

        OUT F = stateOut.getDPhiDt();
        OUT F_ext = *temp2;
        F_ext.StoreAddition(*scaledImpulses, dissipation) *= dt;

        F.Add(F_ext);
    }

    return stateOut;
}

