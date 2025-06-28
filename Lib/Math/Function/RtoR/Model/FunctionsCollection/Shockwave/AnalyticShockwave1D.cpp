//
// Created by joao on 22/04/2021.
//

#include "AnalyticShockwave1D.h"

#include <boost/math/special_functions/lambert_w.hpp>
#include <cmath>


namespace Slab::Math {

    RtoR::AnalyticShockwave1D::AnalyticShockwave1D(DevFloat a0) {
        const auto N = 20;

        ak = {a0};

        Vector<DevFloat> &xk = this->xk;
        auto bk = [&xk](const int k) {
            auto val = 1.;

            for (auto m = 1; m <= k - 1; m++) {
                auto prod = m % 2 ? -1. : 1.;
                for (auto n = 1; n <= m; n++) {
                    auto x_k = xk[k - n - 1];
                    prod *= x_k;
                }
                val += 1. / prod;
            }

            return 2. * val;
        };

        for (int k = 1; k < N + 1; k++) {
            auto b = bk(k);
            //               x = 1 + b*log(x) <-- Lambert W
            //             e^x = e*x^b                         \ (=)^(1/b)
            //         e^(x/b) = e*(1/b)*x                 \ (=)*(-1/b)
            // (-x/b)*e^(-x/b) = -(1/b)e^(-1/b)    \ W_{-1}(=)
            //            -x/b = W_{-1}(-e^(-1/b)/b)
            //               x = -b*W_{-1}(-e^(-1/b)/b)
            auto b_inv_neg = -1. / b;
            auto x = -b * boost::math::lambert_wm1(b_inv_neg * exp(b_inv_neg)); // we use branch -1 of Lambert W.

            xk.push_back(x);
        }

        for (int k = 0; k < N; k++) {
            DevFloat a = a0;
            for (int i = 1; i <= k + 1; i++) a *= xk[i - 1];
            ak.push_back(a);
        }

        zk = {2 * a0};
        for (int k = 1; k <= N; k++) {
            DevFloat z = 2 * ak[k] - zk[k - 1];
            zk.push_back(z);
        }

    }

    DevFloat RtoR::AnalyticShockwave1D::Wk(DevFloat z, int k) const {
        if (k == -1) return z > -ak[0] && z < 0 ? z + ak[0] : .0;

        const auto a_k = ak[k];
        const auto z_k = zk[k];
        const auto ai = -ak[k + 1], af = -ak[k];
        const auto sign = k % 2 ? 1. : -1.;

        if (z < ai || z > af) return .0;

        return sign * (z + a_k + z_k * log(abs(z) / a_k));
    }

    DevFloat RtoR::AnalyticShockwave1D::operator()(DevFloat x) const {
        auto z = .25 * (x * x - t * t);
        auto phi = .0;

        // TODO nao precisa somar todos os Wk, basta descobrir em qual intervalo esta o z e utilizar so aquele Wk;
        auto N = ak.size();
        for (int k = 0; k < N; k++)
            phi += Wk(z, k - 1);

        return phi;
    }

    bool RtoR::AnalyticShockwave1D::isDiscrete() const {
        return false;
    }

    void RtoR::AnalyticShockwave1D::setT(DevFloat t) {
        AnalyticShockwave1D::t = t;
    }

}