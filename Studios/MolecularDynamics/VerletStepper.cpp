//
// Created by joao on 8/08/23.
//

#include <boost/numeric/odeint.hpp>

#include "VerletStepper.h"
#include "Utils/RandUtils.h"
#include "Particle.h"


namespace MolecularDynamics {

    template<class Model>
    VerletStepper<Model>::VerletStepper(Math::NumericConfig &params, Model mechModel)
            : Stepper(), mechanicsModel(mechModel), q(params.getN()), p(params.getN()), state(New<State>(q, p)) {
        const Count N = params.getN();
        const Real L = params.getL();

        if (1) {
            auto lim = (.5 * L - .5 * CUTOFF_RADIUS);

            for (auto &_q: q) {
                auto x = RandUtils::random(-lim, lim);
                auto y = RandUtils::random(-lim, lim);
                _q = Graphics::Point2D(x, y);
            }

            for (auto &_p: p) _p = {0, 0};
        } else {
            int n = sqrt(N);
            auto l = double(L) / n;
            auto d = sqrt(.75) * l;
            Graphics::Point2D base = {-.5 * L, -.5 * L};
            for (auto i = 0; i < n; ++i) {
                for (auto j = 0; (i + n * j) < N; ++j) {
                    q[i + n * j] = base + Graphics::Point2D{d * i + .5 * l,
                                                            l * j + .25 * l + (i % 2) * .5 * l};
                }
            }

            for (auto &_p: p) {
                //_p = (1.e1) * Graphics::Point2D((distr(mtDev) - .5) * 2., (distr(mtDev) - .5) * 2.);
                _p = {.0, .0};
            }
        }
    }

    template<class Model>
    void VerletStepper<Model>::step(const Real &dt, Count n_steps) {
        static boost::numeric::odeint::velocity_verlet<Graphics::PointContainer> stepperVerlet;

        auto pointPairMolecules = std::make_pair(std::ref(q), std::ref(p));

        for (auto i = 0; i < n_steps; ++i) {
            const auto t = dt * (Real) currStep;

            stepperVerlet.do_step(mechanicsModel, pointPairMolecules, t, dt);

            ++currStep;
        }
    }

    template<class Model>
    Math::Base::EquationState_constptr VerletStepper<Model>::getCurrentState() const {
        NOT_IMPLEMENTED
        // return &state;
    }

}