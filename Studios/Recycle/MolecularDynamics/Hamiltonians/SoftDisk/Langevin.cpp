#include "Langevin.h"

#include "SoftDiskParams.h"
#include "Simulation/SimParams.h"

#define FRANDOM (random()/(RAND_MAX+1.0))

Langevin::Langevin(Real dt, Real T) : T(T), dt(dt)
{

}

void Langevin::operator()(const MoleculeContainer &m, MoleculeContainer &dmdt, const Real /* t */)
{
    const Real alpha = sqrt(2*T/dt);

    for(auto &dm : dmdt){
        dm.m_q = alpha*xi();
        dm.m_p = {0., 0.};
    }

    // INTERACTION BETWEEN MOLECULES
    for(size_t i = 0; i<m.size()-1; i++){
        Point2D &dq1 = dmdt[i].m_q, &dp1=dmdt[i].m_p;
        const Point2D &q1 = m[i].m_q, &p1=m[i].m_p;

        for(size_t j=i+1; j<m.size(); j++){
            Point2D &dq2 = dmdt[j].m_q, &dp2=dmdt[j].m_p;
            const Point2D &q2 = m[j].m_q/*, &p2=m[j].m_p*/;

            const Point2D F = mdUdr(q1, q2);

            dq1 = dq1 + F;
            dq2 = dq2 - F;
        }
    }

    // INTERACTION WITH ENGINE
    /*
    const Real halfW = .5*ENGINE_WIDTH;
    const Real halfH = .5*ENGINE_HEIGHT;
    for(size_t i = 0; i<m.size(); i++){
        Point2D &dq = dmdt[i].m_q, &dp = dmdt[i].m_p;
        const Point2D &q = m[i].m_q;

        if(q.x > -halfW && q.x < halfW && q.y > -halfH && q.y < halfH) {
            dq.y += 30;
        }
    }*/
}


Real Langevin::U(const Point2D &q1, const Point2D &q2)
{
    const Real SIGMA_SQR = σ * σ;
    Real normSqr;

    // esses ponto sao percorridos para calcular interacoes considerando a caixa periodica nas
    // oito (08) caixas adjacentes
    const Point2D points[] = {
        q2-q1,

        /*
        Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y)-q1,
        Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y)-q1,
        Point2D(q2.x, q2.y-SIMSPACE_SIDE_SIZE)-q1,
        Point2D(q2.x, q2.y+SIMSPACE_SIDE_SIZE)-q1,

        Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y-SIMSPACE_SIDE_SIZE)-q1,
        Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y+SIMSPACE_SIDE_SIZE)-q1,
        Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y-SIMSPACE_SIDE_SIZE)-q1,
        Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y+SIMSPACE_SIDE_SIZE)-q1
         */
        };

    for(auto r : points){
        normSqr = r.lengthSqr();
        if(normSqr < SIGMA_SQR){
            const Real norm = sqrt(normSqr);

            const DoubleAccess arg = {1- norm / σ};
            return ε / ALPHA * pow(arg.val, ALPHA) * arg.isPositive();
        }
    }

    return 0.0;
}

inline Point2D Langevin::mdUdr(const Point2D &q1, const Point2D &q2)
{
    const Real SIGMA_SQR = σ * σ;
    Real normSqr;

    const Point2D points[] = {
        q2-q1,
        /*
        Point2D(q2.x-SIMSPACE_SIDE_SIZE,    q2.y                   )-q1,
        Point2D(q2.x+SIMSPACE_SIDE_SIZE,    q2.y                   )-q1,
        Point2D(   q2.x,                    q2.y-SIMSPACE_SIDE_SIZE)-q1,
        Point2D(   q2.x,                    q2.y+SIMSPACE_SIDE_SIZE)-q1,

        Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y-SIMSPACE_SIDE_SIZE)-q1,
        Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y+SIMSPACE_SIDE_SIZE)-q1,
        Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y-SIMSPACE_SIDE_SIZE)-q1,
        Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y+SIMSPACE_SIDE_SIZE)-q1
         */
    };

    // GRAVIDADE. Para acrescentar gravidade, basta mudar o valor de 'g':
    const Real g = 0.0;
    Point2D resultForce(0.0, g);

    // COLISAO COM O CHAO:
    //const double hw = SIMSPACE_SIDE_SIZE*.5;
    //if(q1.y-σ < -hw) {
    //    //resultForce.y -= 0.5*(q1.y-σ+hw);
    //    resultForce.y -= K_SPRING*(q1.y-σ - hw);
    //}
    // COLISAO COM O TETO:
    //if(q1.y+σ > +hw) {
    //    //resultForce.y -= 0.5*(q1.y-σ+hw);
    //    resultForce.y -= K_SPRING*(q1.y+σ + hw);
    //}

    for(auto r : points){
        normSqr = r.lengthSqr();

        if(normSqr < SIGMA_SQR){
            const Real norm = sqrt(normSqr);

            const DoubleAccess arg = {1.0- norm / σ};
            const Real mag = ε / σ * pow(arg.val, ALPHA - 1.0) * arg.isPositive();
            resultForce = resultForce - (mag/norm)*r;

            //resultForce = resultForce + K_SPRING*(1.0-2.0*σ/length)*r;
        }
    }

    return resultForce;
}

Point2D Langevin::xi()
{
    const Real z = FRANDOM, theta=2.0*M_PI*FRANDOM;
    const Real r=sqrt(-2.0*log(1.0-z));

    return {r*cos(theta), r*sin(theta)};
}





