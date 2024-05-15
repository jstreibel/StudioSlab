//

#include "AbsPerturbedFunction.h"

using namespace RtoR;

inline floatt Hn(floatt arg){
    return UnitStep(arg+2.0) - UnitStep(arg-2.0);
}

AbsPerturbedDiffFunction::AbsPerturbedDiffFunction(UInt Ns) : Ns(Ns) {

}

Real AbsPerturbedDiffFunction::operator()(Real x) const {
    floatt Vp=0;
    for(int n = -Ns; n<=Ns; ++n){
        const floatt arg = x - 4.0*floatt(n);
        Vp += (SIGN(arg) - arg)*Hn(arg);
    }
    return Vp;}

Core::FunctionT<Real, Real>::Ptr AbsPerturbedDiffFunction::diff(int n) const {
    return FunctionT<Real, Real>::Ptr(new AbsPerturbedDiffFunction(Ns));
}

Core::FunctionT<Real, Real> *AbsPerturbedDiffFunction::Clone() const {
    return new AbsPerturbedDiffFunction(Ns);
}

//
// Created by joao on 13/09/2019.
AbsPerturbedFunction::AbsPerturbedFunction(UInt Ns) : Ns(Ns) {

}

Real AbsPerturbedFunction::operator()(Real x) const {
    floatt V=0;
    for(int n = -Ns; n<=Ns; ++n){
        const floatt arg = x - 4.0*floatt(n);
        V += (ABS(arg) - 0.5*arg*arg)*Hn(arg);
    }

    return V;
}

Core::FunctionT<Real, Real>::Ptr AbsPerturbedFunction::diff(int n) const {
    return FunctionT<Real, Real>::Ptr(new AbsPerturbedDiffFunction(Ns));
}

Core::FunctionT<Real, Real> *AbsPerturbedFunction::Clone() const {
    return new AbsPerturbedFunction(Ns);
}

Real AbsPerturbedFunction::Fold(Real eta, const int Ns)
{
    floatt etab = 0;

    for(int n=-Ns; n<=Ns; ++n)
        etab += fabs(eta-4*floatt(n))*Hn(eta-4*floatt(n));

    return etab;
}