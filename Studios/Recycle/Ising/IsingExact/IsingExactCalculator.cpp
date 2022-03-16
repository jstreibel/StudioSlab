//
// Created by joao on 15/05/2021.
//

#include "IsingExactCalculator.h"
#include "../Utils/ThermoUtils.h"

ExactIsingCalculator::ExactIsingCalculator(int L) : S(L) {}

void ExactIsingCalculator::ComputeAll(double T) {
    const auto N_ = double(S.N);
    const auto invN = 1./N_;
    const auto invN2 = invN*invN;
    std::cout.precision(8);
    const auto beta = 1./T;

    double Z_=0, E_avg=0, E2_avg=0, M_avg=0, M2_avg=0;
    for(S=0; S<S.NStates(); ++S){
        const double E_ = S.E();
        const double M_ = S.M();

        const double w = ThermoUtils::BoltzmannWeight_betaE(E_ * beta);

        Z_ += w;

        E_avg  += w*E_;
        E2_avg += w*E_*E_;

        M_avg += w*fabs(M_);

        M2_avg += w*M_*M_;
    }

    const double invZ = 1./Z_;

    E_avg *= invZ;
    E2_avg *= invZ;
    M_avg *= invZ;
    M2_avg *= invZ;

    const double e = E_avg*invN;                // ok
    const double e2 = E2_avg*invN2;
    const double m = M_avg*invN;                // ok
    const double m2 = M2_avg*invN2;

    //const double f = -T*(log(Z_)) * invN;       // ok
    //const double c_v =N_*beta*beta*(e2-e*e);    // ok
    //const double chi =   beta*N_*(m2-m*m);      // ok

    const double f =   ThermoUtils::f_Gibbs(T, Z_, N_);       // ok
    const double c_v = ThermoUtils::c_v(T, e, e2, N_);    // ok
    const double chi = ThermoUtils::chi(T, m, m2, N_);      // ok

    std::cout << std::scientific
              << T << " "       //  1
              << Z_ << " "      //  2
              << e << " "       //  3
              << m << " "       //  4
              << c_v << " "     //  5
              << chi << " "     //  6
              << f << " "       //  7
              << e2 << " "      //  8
              << m2 << " "      //  9
              << e*e << " "     // 10
              << m*m << " "     // 11
              << "\n";
}
