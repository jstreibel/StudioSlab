#include "Simulation.h"
#include "SimParams.h"
#include "Hamiltonians/NewtonMechanicsParams.h"

#include "IO/UserInput.h"

#include <boost/numeric/odeint.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_real_distribution.hpp>

Simulation::Simulation(size_t N, BackendSFML *output)
    : q(N), p(N), output(output)
{

    boost::mt19937 mtDev(3);
    boost::random_device randomDev;
    boost::random::uniform_real_distribution<> distr;

    const Real L = SPACE_L;


    if(1){
        for(auto &_q : q)
            _q = (.5*L - .5 * CUTOFF_RADIUS) * Point2D((distr(mtDev) - .5) * 2., (distr(mtDev) - .5) * 2.);

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

}

void Simulation::KeyPressed(sf::Event::KeyEvent &keyEvent, int value) {
    switch (keyEvent.code){
        case sf::Keyboard::Escape:
            isFinished = true;
            break;
        case sf::Keyboard::Space:
            for(auto &_p:p){
                _p.x *= 0.1;
                _p.y *= 0.1;
            }
            break;
    }
}


void Simulation::run() {
    using namespace boost::numeric::odeint;

    UserInput *input = UserInput::getInstance();

    velocity_verlet<std::vector<Point2D>> stepperVerlet;
    runge_kutta4<MoleculeContainer> stepperRK4;

    size_t steps;
    {
        isFinished = false;

        Real t=0.0;
        steps = 0;
        for(; !isFinished; t+=dt, steps++)
        {
            auto molecules = std::make_pair(std::ref(q), std::ref(p));
            stepperVerlet.do_step(physModelMolDynamic, molecules, t, dt);

            input->treatEvents();
            if(!(steps%10)){
                (*output)(q, physModelMolDynamic.flippedSides);

                auto fastest = p[0];
                auto velSqr = fastest.lengthSqr();
                auto _i=0;
                for(auto i=1; i<p.size(); ++i){
                    auto _velSqr = p[i].lengthSqr();

                    if(_velSqr > velSqr){
                        fastest = p[i];
                        _i=i;
                    }
                }

                //std::cout << "t=" << t << "\t\tfastest: " <<_i<< "\tq=(" << q[_i].x << ",\t" << q[_i].y << ")" << "\t\t|p|=" << sqrt(velSqr) << std::endl;

                if(_i==11 && 0){
                    std::cout << "BREAK";
                }
            }
        }
    }
}
