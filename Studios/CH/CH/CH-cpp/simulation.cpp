#include "simulation.h"

#include <boost/numeric/odeint.hpp>
#include "outputopengl.h"

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_real_distribution.hpp>

Simulation::Simulation(size_t N, const double h, const CHData::DConfig dconfig) : /*molecules(N),*/ phi(N, h, dconfig)
{
    boost::random_device dev;
    boost::random::uniform_real_distribution<> d;

    for(double &s : phi){
        s = 0.1+0.01*d(dev);
    }

    /*
    const double L = SIM_AREA;

    Point2D q, p;
    for(auto &m : molecules){
        q = .25*L*Point2D((d(dev)-.5)*2., (d(dev)-.5)*2.);
        p = 0.1*Point2D(d(dev)-.5, d(dev)-.5);
        m.q = q;
        m.p = p;
    }

    if(1) {
        const double dist = 0.8;

        molecules[0].q = Point2D(0,  dist*.5);
        molecules[1].q = Point2D(0, -dist*.5);
        molecules[0].p = Point2D(-0.1, 0.0);
        molecules[1].p = Point2D( 0.1, 0.0);
    }

    */
}

void Simulation::run() {
    using namespace boost::numeric::odeint;

    const double T = 1.e-2;
    const double dt = 1.e-3;

    OutputOpenGL out(1.0);

    //Langevin physModel(T, dt);
    //runge_kutta4<MoleculContainer> stepper;

    // Liu
    const double A_L [] = {37100.0, 2869.0, 3251.0};
    const double B_L [] = {-5.194, 0, 0};

    // Palumbo:
    const double A_P [] = {44537.0, -7489.0, 4390.0}; (void)A_P;
    const double B_P [] = {-10.095, 4.161, 0}; (void)B_P;

    // Turchanin:
    const double A_T [] = {34600, -6410, 4390}; (void)A_T;
    const double B_T [] = {-4, 3.7, 0}; (void)B_T;

    const double *A = A_L,
                 *B = B_L;

    CahnHilliard cahnHilliard(T, A[0], A[1], A[2],
                                 B[0], B[1], B[2]);
    runge_kutta4<CHData> stepper;

    size_t steps;
    {
        double t=0.0;
        steps = 0;
        for(; !out.isFinished(); t+=dt, steps++)
        {
            stepper.do_step(cahnHilliard, phi, t, dt);
            cahnHilliard.apply_bc(phi, t);
            out(phi, t);

            //stepper.do_step(physModel, molecules, t , dt );
            //physModel.apply_bc(molecules);
            //out(molecules, t);
        }
    }
}
