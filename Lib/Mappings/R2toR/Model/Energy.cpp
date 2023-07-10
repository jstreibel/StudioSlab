//
// Created by joao on 29/05/23.
//
#include "Energy.h"

#include "Phys/Thermal/Utils/RandUtils.cpp"

Real Phys::Gordon::Energy::operator[](const R2toR::EquationState &function) const {
    const auto &phi = function.getPhi();
    const auto &dphidt = function.getDPhiDt();
    const auto &phiSpace = phi.getSpace();

    auto h = phiSpace.geth();
    auto inv_2h = .5/h;
    auto h2 = h*h;
    auto N1 = phi.getN();
    auto N2 = phi.getM();

    auto K = .0;
    auto D = .0;
    auto V = .0;

    for(auto i=1; i<N1-1; ++i){
        for(auto j=1; j<N2-1; ++j){
            auto C = phi.   At(i  , j  );
            auto N = phi.   At(i  , j-1);
            auto S = phi.   At(i  , j+1);
            auto E = phi.   At(i+1, j  );
            auto W = phi.   At(i-1, j  );
            auto p = dphidt.At(i,j);

            auto dx = inv_2h*( E-W );
            auto dy = inv_2h*( N-S );

            K += .5*p*p*h2;
            D += .5*(dx*dx + dy*dy)*h2;
            V += fabs(C)*h2;
        }
    }

    return K+D+V;
}

Real Phys::Gordon::Energy::computeRadial_method1(const R2toR::EquationState &function, Real upToRadius) {
    const auto &phi = function.getPhi();
    const auto &dphidt = function.getDPhiDt();
    const auto &phiSpace = phi.getSpace();

    auto h = phiSpace.geth();
    auto inv_2h = .5/h;
    auto h2 = h*h;
    auto N1 = phi.getN();
    auto N2 = phi.getM();

    const auto R = upToRadius;
    const auto nRadius    = R/h;
    const auto nRadiusSqr = nRadius*nRadius;
    const auto iCenter    = N1/2 + N1%2;
    const auto jCenter    = N2/2 + N2%2;

    auto K = .0;
    auto D = .0;
    auto V = .0;

    for(auto i=1; i<N1-1; ++i){
        for(auto j=1; j<N2-1; ++j){
            const auto i_c = i-iCenter;
            const auto j_c = j-jCenter;
            if((i_c*i_c + j_c*j_c) > nRadiusSqr) continue;

            auto C = phi.   At(i  , j  );
            auto N = phi.   At(i  , j-1);
            auto S = phi.   At(i  , j+1);
            auto E = phi.   At(i+1, j  );
            auto W = phi.   At(i-1, j  );
            auto p = dphidt.At(i,j);

            auto dx = inv_2h*( E-W );
            auto dy = inv_2h*( N-S );

            K += .5*p*p*h2;
            D += .5*(dx*dx + dy*dy)*h2;
            V += fabs(C)*h2;
        }
    }

    return K+D+V;
}

Real Phys::Gordon::Energy::computeRadial_method2(const R2toR::EquationState &function, Real upToRadius) {
    const auto &phi = function.getPhi();
    const auto &dphidt = function.getDPhiDt();

    assert(phi.getN() == phi.getM());

    auto h = phi.getSpace().geth();
    auto inv_2h = .5/h;
    auto h2 = h*h;
    auto N_ = phi.getN();
    //auto L = phi.getDomain().getLx();
    auto R = upToRadius;

    auto K = .0;
    auto D = .0;
    auto V = .0;

    const auto nRadius    = R/h;
    const auto nRadiusSqr = nRadius*nRadius;
    const auto iCenter    = N_/2 + N_%2;
    const auto jCenter    = iCenter;

    auto iMin = iCenter-nRadius;
    auto iMax = iCenter+nRadius+1;
    if(nRadius>iCenter) { iMin=0; iMax=N_; }

    for(auto i=iMin; i<=iMax; ++i){
        const auto iDelta = i-iCenter;
        const auto delta = sqrtl(nRadiusSqr - iDelta*iDelta);

        const auto jMax = jCenter + delta + 1;
        const auto jMin = jCenter - delta;

        for(auto j=jMin; j<jMax; ++j){
            auto C = phi.   At(i  , j  );
            auto N = phi.   At(i  , j-1);
            auto S = phi.   At(i  , j+1);
            auto E = phi.   At(i+1, j  );
            auto W = phi.   At(i-1, j  );
            auto p = dphidt.At(i,j);

            auto dx = inv_2h*( E-W );
            auto dy = inv_2h*( N-S );

            K += p*p;
            D += dx*dx + dy*dy;
            V += fabs(C);
        }
    }

    return (.5*(K+D)+V)*h2;
}