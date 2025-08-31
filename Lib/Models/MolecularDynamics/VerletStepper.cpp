//
// Created by joao on 8/08/23.
//

#include <boost/numeric/odeint.hpp>

#include "VerletStepper.h"
#include "Utils/RandUtils.h"
#include "Models/MolecularDynamics/Hamiltonians/Particle.h"


namespace Slab::Models::MolecularDynamics {

    template<class Model>
    TVerletStepper<Model>::TVerletStepper(Config c, Model mechModel)
            : FStepper()
            , MechanicsModel(mechModel)
            , q(c->getN()), p(c->getN()), State(New<FMoleculesState>(q, p))
            , dt(c->getdt()){
        const CountType N = c->getN();
        const DevFloat L = c->GetL();

        if constexpr  (true) {
            auto lim = (.5 * L - .5 * CUTOFF_RADIUS);

            for (auto &_q: q) {
                auto x = RandUtils::RandomUniformReal(-lim, lim);
                auto y = RandUtils::RandomUniformReal(-lim, lim);
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
    void TVerletStepper<Model>::Step(CountType n_steps) {
        static boost::numeric::odeint::velocity_verlet<Graphics::PointContainer> StepperVerlet;

        auto PointPairMolecules = std::make_pair(std::ref(q), std::ref(p));

        for (auto i = 0; i < n_steps; ++i) {
            const auto t = dt * (DevFloat) currStep;

            StepperVerlet.do_step(MechanicsModel, PointPairMolecules, t, dt);

            ++currStep;
        }
    }

    template<class Model>
    Math::Base::EquationState_constptr TVerletStepper<Model>::GetCurrentState() const {
        return State;
    }

}