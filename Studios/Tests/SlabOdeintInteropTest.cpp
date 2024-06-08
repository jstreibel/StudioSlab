//
// Created by joao on 6/1/24.
//

#include "Math/Numerics/Odeint/OdeintSolver.h"
#include "Math/Numerics/Odeint/OdeintState.h"
#include "Math/Numerics/Odeint/OdeintToSlabState.h"

class SolverTest : public Slab::Math::Odeint::OdeintSolver {
public:
    SolverTest() = default;

    void operator()(const Slab::Math::Odeint::OdeintStateWrapper &x,
                          Slab::Math::Odeint::OdeintStateWrapper &dxdt,
                    double t) override {

    }
};

class StateyState : public Slab::Math::Odeint::OdeintState<Slab::Real> {

};


int main() {

    using namespace Slab;
    using namespace Math;

    Base::OdeintToSlabState statey;

    Odeint::StepperRK4 stepper;

    double t = 0.0;
    double dt = 0.01;

    SolverTest solvyDolvy;

    /*
    boost::numeric::odeint::integrate_const(stepper, solvyDolvy, x, t, 10.0, dt,
                                            [](const Odeint::OdeintStateWrapper& x, double t) {
                                                std::cout << "t = " << t << ": ";
                                                x.print();
                                            });
                                            */

    return 0x54235;
}