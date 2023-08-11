//
// Created by joao on 8/08/23.
//

#include <boost/numeric/odeint.hpp>

#include "Stepper.h"
#include "Common/RandUtils.h"
#include "Particle.h"

const Real Temperature = 1e-3;

MolecularDynamics::VerletStepper::VerletStepper(NumericParams &params)
: Stepper()
, lennardJones(params)
, softDisk(params, Temperature)
, molecules(params.getN())
, q(params.getN())
, p(params.getN())
, state(q,p)
{
    const Count N = params.getN();
    const Real L = params.getL();

    if(1){
        auto lim = (.5 * L - .5 * CUTOFF_RADIUS);

        for(auto &_q : q) {
            auto x = RandUtils::random(-lim,lim);
            auto y = RandUtils::random(-lim,lim);
            _q = Point2D(x, y);
        }

        for(auto &_p : p) _p = {0, 0};
    }

    else {
        int n = sqrt(N);
        auto l = double(L) / n;
        auto d = sqrt(.75) * l;
        Point2D base = {-.5 * L, -.5 * L};
        for (auto i = 0; i < n; ++i) {
            for (auto j = 0; (i + n * j) < N; ++j) {
                q[i + n * j] = base + Point2D{d * i + .5 * l,
                                              l * j + .25 * l + (i % 2) * .5 * l};
            }
        }

        for (auto &_p : p) {
            //_p = (1.e1) * Point2D((distr(mtDev) - .5) * 2., (distr(mtDev) - .5) * 2.);
            _p = {.0, .0};
        }
    }

    for(Count i=0; i<params.getN(); ++i){
        auto &m = molecules[i];

        m = {q[i], p[i]};
    }
}

void MolecularDynamics::VerletStepper::step(const Real &dt, Count n_steps) {
    static boost::numeric::odeint::velocity_verlet<PointContainer> stepperVerlet;

    for(auto i=0; i<n_steps; ++i) {
        const auto t = dt * (Real)currStep;

        auto pointPairMolecules = std::make_pair(std::ref(q), std::ref(p));

        if(0) {
            stepperVerlet.do_step(lennardJones, pointPairMolecules, t, dt);
        } else {
            stepperVerlet.do_step(softDisk, pointPairMolecules, t, dt);
        }

        ++currStep;
    }
}

const void *MolecularDynamics::VerletStepper::getCurrentState() const {
    return &state;
}

DiscreteSpacePair MolecularDynamics::VerletStepper::getSpaces() const {
    return DiscreteSpacePair();
}

