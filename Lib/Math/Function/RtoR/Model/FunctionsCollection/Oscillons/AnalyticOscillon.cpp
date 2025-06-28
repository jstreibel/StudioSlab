//
// Created by joao on 09/09/2019.
//

#include "AnalyticOscillon.h"

#include "analytic-breather.h"
#include "Math/Formalism/Categories.h"
#include <cmath>

namespace Slab::Math::RtoR {

    auto gamma(DevFloat u) -> DevFloat {
        return 1. / sqrt(1. - u * u);
    }

    auto ab(DevFloat alpha, DevFloat u, DevFloat v) -> RealPair {
        const DevFloat alpha_0 = .5 * (1. - u * u);

        // assert(!(alpha < 0. || alpha > 1.));

        if (alpha < alpha_0) return {1., 0.};

        return {-1., 1.};
    }

    auto x_0(DevFloat alpha, DevFloat u, DevFloat v) -> DevFloat {
        RealPair p_ab = ab(alpha, u, v);
        DevFloat a = p_ab.first;
        DevFloat b = p_ab.second;

        // -gamma * (alpha * u);
        return -gamma(u) / (1. + a * v * u) * ((1. - u * u) * v * b + alpha * (u + v * a));
    }

    auto t_0(DevFloat alpha, DevFloat u) -> DevFloat {
        return gamma(u) * alpha;
    }

    // AnalyticOscillon::AnalyticOscillon(const AnalyticOscillon &copy)
    // : t0(copy.t0), x0(copy.x0), l(copy.l), v(copy.v), u(copy.u), alpha(copy.alpha)
    // , s1(copy.s1), s2(copy.s2), bit(copy.bit)
    // {    }

    AnalyticOscillon::AnalyticOscillon(DevFloat t, DevFloat v, DevFloat u, DevFloat alpha, bool xMirrored, bool phiMirrored, Bit bit)
            : t0(t_0(alpha, -u) - t), x0(x_0(alpha, -u, v)), l(1), v(v), u(u), s1(phiMirrored ? -1 : 1),
              s2(xMirrored ? -1 : 1), alpha(alpha), bit(bit) {

    }

    AnalyticOscillon::AnalyticOscillon(DevFloat t, DevFloat x0, DevFloat l, DevFloat v, DevFloat u, DevFloat alpha, bool xMirrored,
                                       bool phiMirrored, AnalyticOscillon::Bit bit)
            : t0(t_0(alpha, -u) - t), x0(x_0(alpha, -u, v) + x0), l(l), v(v), u(u), s1(phiMirrored ? -1 : 1),
              s2(xMirrored ? -1 : 1), alpha(alpha), bit(bit) {

    }

    auto AnalyticOscillon::operator()(DevFloat x) const -> DevFloat {
        if (bit == dPhiDt) return s1 * l * dpsi(t0 / l, (s2 * x - x0) / l, v, -u);

        return s1 * l * l * psi(t0 / l, (s2 * x - x0) / l, v, -u);
    }

    auto AnalyticOscillon::Clone() const -> Pointer<FunctionT> {
        return New<AnalyticOscillon>(*this);
    }

    auto AnalyticOscillon::swap() -> AnalyticOscillon & {
        bit = (bit == phi ? dPhiDt : phi);

        return *this;
    }

    void AnalyticOscillon::set_t(DevFloat t) { t0 = (t_0(alpha, -u) - t); }

    void AnalyticOscillon::setBit(AnalyticOscillon::Bit bitty) {
        bit = bitty;
    }

    Str AnalyticOscillon::symbol() const {
        if(bit == dPhiDt) return "𝜕ₜOₗ(t,x)";

        return "Oₗ(t,x)";
    }

    Str AnalyticOscillon::generalName() const {
        return "Oscillon";
    }


}