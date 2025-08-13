//
// Created by joao on 03/12/2019.
//

#include "AnalyticShockwave2DRadialSymmetry.h"



namespace Slab::Math {

//RtoR::AnalyticShockwave2DRadialSymmetry::AnalyticShockwave2DRadialSymmetry(Real a0) : a0(a0) {
//
//}

    DevFloat RtoR::AnalyticShockwave2DRadialSymmetry::W_k(UInt k, DevFloat z) const {
        if (k >= quant) return .0;
        if (z > .0) return .0;

        const DevFloat c = 0.666666666666666;

        if (k == 0)
            return c * (z + a_k[0]);

        const DevFloat a = a_k[k - 1];

        if (z == .0) return .0;
        if (k % 2) // impar
            return -c * (z + a) + beta_k[k] * (1. / sqrt(-z) - 1. / sqrt(a));
        else
            return c * (z + a) + beta_k[k] * (1. / sqrt(-z) - 1. / sqrt(a));
    }

    DevFloat RtoR::AnalyticShockwave2DRadialSymmetry::theta_k(UInt k, DevFloat z) const {
        if (!k) return UnitStep(-z) * UnitStep(-(-z - a_k[0]));

        return UnitStep(-(z + a_k[k - 1])) * UnitStep(z + a_k[k]);
    }

    DevFloat RtoR::AnalyticShockwave2DRadialSymmetry::operator()(DevFloat r) const {
        DevFloat z = .25 * (r * r - t * t);

        DevFloat sum = .0;

        for (UInt n = 0; n < quant; n++)
            sum += theta_k(n, z) * W_k(n, z);

        return sum;
    }

    RtoR::AnalyticShockwave2DRadialSymmetry::AnalyticShockwave2DRadialSymmetry(DevFloat a0) {
        for (auto i = 0; i < quant; ++i) {
            auto &a = a_k[i];
            auto &beta = beta_k[i];
            a *= a0;


        }
    }


    DevFloat RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivative::dW_kdz(UInt k, DevFloat z) const {
        if (k >= quant) return .0;
        if (z > .0) return .0;

        constexpr const DevFloat c = 2 / 3.;

        if (k == 0) return c;

        const DevFloat a = a_k[k - 1];
        if (k % 2) // impar
            return -c + beta_k[k] * .5 * pow(-z, 1.5);
        else
            return c + beta_k[k] * .5 * pow(-z, 1.5);
    }

    DevFloat RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivative::theta_k(UInt k, DevFloat z) const {
        if (!k) return UnitStep(-z) * UnitStep(-(-z - a_k[0]));

        return UnitStep(-(z + a_k[k - 1])) * UnitStep(z + a_k[k]);
    }

    DevFloat RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivative::operator()(DevFloat r) const {
        DevFloat z = .25 * (r * r - t * t);
        DevFloat dzdt = -.5 * t;

        DevFloat sum = .0;

        for (UInt n = 0; n < quant; n++)
            sum += theta_k(n, z) * dzdt * dW_kdz(n, z);

        return sum;
    }


}