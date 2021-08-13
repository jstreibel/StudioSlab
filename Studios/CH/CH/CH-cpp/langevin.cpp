#include "langevin.h"

#define FRANDOM (random()/(RAND_MAX+1.0))

/*
Langevin::Langevin(double T, double dt) : T(T), dt(dt)
{

}

void Langevin::apply_bc(MoleculContainer &mols)
{
    const double hw = SIM_AREA*.5;
    for(auto &m : mols){
        Point2D &q = m.q;
        if      (q.x < -hw) q.x += SIM_AREA;
        else if (q.x >  hw) q.x -= SIM_AREA;

        if      (q.y < -hw) q.y += SIM_AREA;
        else if (q.y >  hw) q.y -= SIM_AREA;
    }
}

double Langevin::computeEnergy(const std::vector<Point2D> &v_q, std::vector<Point2D> &v_p)
{
    double K = .0;
    for(auto &p : v_p)
        K += p.normSqr();
    K *= .5;

    const size_t N = v_q.size();
    double U = 0.0;
    for(size_t i=0; i<N-1; i++){
        const Point2D &q1 = v_q[i];
        for(size_t j=i; j<N; j++){
            const Point2D &q2 = v_q[j];

            U += _U(q1, q2);
        }
    }

    return K + U;
}

void Langevin::operator()(const MoleculContainer &m, MoleculContainer &dmdt, const double t)
{

    const double alpha = sqrt(2*T/dt);

    for(auto &dm : dmdt){
        dm.q = alpha*_xi();
        dm.p = {0., 0.};
    }

    for(size_t i = 0; i<m.size()-1; i++){
        Point2D &dq1=dmdt[i].q, &dp1=dmdt[i].p;
        const Point2D &q1=m[i].q, &p1=m[i].p;

        for(size_t j=i+1; j<m.size(); j++){
            Point2D &dq2=dmdt[j].q, &dp2=dmdt[j].p;
            const Point2D &q2=m[j].q, &p2=m[j].p;

            const Point2D F = _mdUdr(q1, q2);

            dq1 = dq1 + F;
            dq2 = dq2 - F;
        }
    }
}

union DoubleAccess {
    double val;
    char bytes[8];

    inline bool isPositive() const {
        return !((bytes[7] & 0x80) >> 7);

    }
};

double Langevin::_U(const Point2D &q1, const Point2D &q2)
{
    const double SIGMA_SQR = SIGMA*SIGMA;
    double normSqr;

    const Point2D points[] = {
        q2-q1,

        Point2D(q2.x-SIM_AREA, q2.y)-q1,
        Point2D(q2.x+SIM_AREA, q2.y)-q1,
        Point2D(q2.x, q2.y-SIM_AREA)-q1,
        Point2D(q2.x, q2.y+SIM_AREA)-q1,

        Point2D(q2.x-SIM_AREA, q2.y-SIM_AREA)-q1,
        Point2D(q2.x-SIM_AREA, q2.y+SIM_AREA)-q1,
        Point2D(q2.x+SIM_AREA, q2.y-SIM_AREA)-q1,
        Point2D(q2.x+SIM_AREA, q2.y+SIM_AREA)-q1
        };

    for(auto r : points){
        normSqr = r.normSqr();
        if(normSqr < SIGMA_SQR){
            const double norm = sqrt(normSqr);
            const DoubleAccess arg = {1-norm/SIGMA};

            return EPSILON/ALPHA * pow(arg.val, ALPHA) * arg.isPositive();
        }
    }

    return 0.0;
}

inline Point2D Langevin::_mdUdr(const Point2D &q1, const Point2D &q2)
{
    const double SIGMA_SQR = SIGMA*SIGMA;
    double normSqr;

    const Point2D points[] = {
        q2-q1,

        Point2D(q2.x-SIM_AREA, q2.y)-q1,
        Point2D(q2.x+SIM_AREA, q2.y)-q1,
        Point2D(q2.x, q2.y-SIM_AREA)-q1,
        Point2D(q2.x, q2.y+SIM_AREA)-q1,

        Point2D(q2.x-SIM_AREA, q2.y-SIM_AREA)-q1,
        Point2D(q2.x-SIM_AREA, q2.y+SIM_AREA)-q1,
        Point2D(q2.x+SIM_AREA, q2.y-SIM_AREA)-q1,
        Point2D(q2.x+SIM_AREA, q2.y+SIM_AREA)-q1
    };

    for(auto r : points){
        normSqr = r.normSqr();

        if(normSqr < SIGMA_SQR){
            const double norm = r.norm();
            const DoubleAccess arg = {1.0-norm/SIGMA};
            const double mag = EPSILON/SIGMA * pow(arg.val, ALPHA-1.0) * arg.isPositive();

            return -(mag/norm)*r;
        }
    }

    return {0.,0.};
}

Point2D Langevin::_xi()
{
    const double z = FRANDOM, theta=2.0*M_PI*FRANDOM;
    const double r=sqrt(-2.0*log(1.0-z));

    return {r*cos(theta), r*sin(theta)};
}

*/
