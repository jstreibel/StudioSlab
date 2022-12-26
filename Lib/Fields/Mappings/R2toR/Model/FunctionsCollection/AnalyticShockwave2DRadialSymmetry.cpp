//
// Created by joao on 03/12/2019.
//

#include "AnalyticShockwave2DRadialSymmetry.h"


//RtoR::AnalyticShockwave2DRadialSymmetry::AnalyticShockwave2DRadialSymmetry(Real a0) : a0(a0) {
//
//}

Real RtoR::AnalyticShockwave2DRadialSymmetry::W_k(PosInt k, Real z) const {
    if(k>=quant) return .0;
    if(z>.0) return .0;

    const Real c = 0.666666666666666;

    if(k==0)
        return c*(z+a_k[0]);

    const Real a = a_k[k-1];

    if(z==.0) return .0;
    if(k%2) // impar
        return -c*(z+a) + beta_k[k] * (1./sqrt(-z) - 1./sqrt(a));
    else
        return  c*(z+a) + beta_k[k] * (1./sqrt(-z) - 1./sqrt(a));
}

Real RtoR::AnalyticShockwave2DRadialSymmetry::theta_k(PosInt k, Real z) const {
    if(!k) return UnitStep(-z)*UnitStep(-(-z-a_k[0]));

    return UnitStep(-(z+a_k[k-1]))*UnitStep(z+a_k[k]);
}

Real RtoR::AnalyticShockwave2DRadialSymmetry::operator()(Real r) const {
    Real z = .25*(r*r-t*t);

    Real sum=.0;

    for(PosInt n=0; n<quant; n++)
        sum += theta_k(n, z)*W_k(n, z);

    return sum;
}



Real RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivative::dW_kdz(PosInt k, Real z) const {
    if(k>=quant) return .0;
    if(z>.0) return .0;

    const Real c = 0.666666666666666;

    if(k==0) return c;

    const Real a = a_k[k-1];
    if(k%2) // impar
        return -c + beta_k[k]*.5*pow(-z,1.5);
    else
        return  c + beta_k[k]*.5*pow(-z,1.5);
}

Real RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivative::theta_k(PosInt k, Real z) const {
    if(!k) return UnitStep(-z)*UnitStep(-(-z-a_k[0]));

    return UnitStep(-(z+a_k[k-1]))*UnitStep(z+a_k[k]);
}

Real RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivative::operator()(Real r) const {
    Real z = .25*(r*r-t*t);
    Real dzdt = -.5*t;

    Real sum=.0;

    for(PosInt n=0; n<quant; n++)
        sum += theta_k(n, z)*dzdt*dW_kdz(n, z);

    return sum;
}
