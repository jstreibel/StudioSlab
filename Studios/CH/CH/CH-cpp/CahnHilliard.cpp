#include "CahnHilliard.h"

CahnHilliard::CahnHilliard(double T, double A1, double A2, double A3, double B1, double B2, double B3)
    : T(T),
      A1(A1), A2(A2), A3(A3),
      B1(B1), B2(B2), B3(B3)
{
}

void CahnHilliard::operator()(const CHData &phi, CHData &dphidt, const double /* t */)
{
    for(size_t Cn=0; Cn<phi.size(); Cn++){
        dphidt[Cn] = -2.0*calcCahnHilliard(phi, int(Cn));
    }
}

void CahnHilliard::apply_bc(CHData &phi, const double t)
{
    (void)phi;
    (void)t;
}

double CahnHilliard::calcCahnHilliard(const CHData &phi, int Cn)
{
    const double _num = num(phi, T, Cn);
    const double _den = den(phi, T, Cn);

    return _num / _den;
}

double CahnHilliard::num(const CHData &phi, double T, int Cn)
{
    double c = phi[size_t(Cn)];
    double dc = phi.getD1Val(Cn);
    double ddc = phi.getD2Val(Cn);
    //double dddc = phi.getD3Val(Cn);
    double ddddc = phi.getD4Val(Cn);

    return  (-0.000171553*T + 0.000343105*T*c +
                    ( 0.000247729*A2 + 0.000990918*A3 + 0.000247729*B2*T + 0.000990918*B3*T)*c*c +
                    (-0.000495459*A2 - 0.003963670*A3 - 0.000495459*B2*T - 0.003963670*B3*T)*c*c*c +
                    ( 0.000247729*A2 + 0.004954590*A3 + 0.000247729*B2*T + 0.004954590*B3*T)*c*c*c*c +
                    (                - 0.001981840*A3                    - 0.001981840*B3*T)*c*c*c*c*c
            )*dc*dc

    +
            c*
            (
             (0.000171553*T
                + (-0.0000412882*A1 - 0.0001238650*A2 - 0.000206441*A3 - 0.0001715530*T    - 0.0000412882*B1*T - 0.0001238650*B2*T - 0.000206441*B3*T)*c
                + ( 0.0000825765*A1 + 0.0004954590*A2 + 0.001403800*A3                     + 0.0000825765*B1*T + 0.0004954590*B2*T + 0.001403800*B3*T)*c*c
                + (-0.0000412882*A1 - 0.0006193230*A2 - 0.003179190*A3                     - 0.0000412882*B1*T - 0.0006193230*B2*T - 0.003179190*B3*T)*c*c*c
                + (                   0.0002477290*A2 + 0.002972750*A3                                         + 0.0002477290*B2*T + 0.002972750*B3*T)*c*c*c*c
                + (                                   - 0.000990918*A3                                                             - 0.000990918*B3*T)*c*c*c*c*c)
                *ddc + 0.010615*(1.0 - c)*(1.0 - c)*c*ddddc);
}

double CahnHilliard::den(const CHData &phi, double T, int Cn)
{
    double c = phi[size_t(Cn)];
    return exp(2987.88/T)*T*(c-1)*(c-1)*c*c;
}
