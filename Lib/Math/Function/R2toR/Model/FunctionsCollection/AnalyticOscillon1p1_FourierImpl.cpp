//
// Created by joao on 10/5/24.
//

#include "AnalyticOscillon1p1_FourierImpl.h"


namespace Slab::Math::R2toR {

    DevFloat phi_n(Int n, DevFloat t, DevFloat x) {
        auto factor = 1./(DevFloat)(n*n*n);
        auto npi = n*M_PI;
        auto cx = cos(npi*x);
        auto ct = cos(2*npi*t);
        return factor*ct*cx*cx;
    }

    DevFloat ddtphi_n(Int n, DevFloat t, DevFloat x) {
        auto factor = -2.*M_PI/(DevFloat)(n*n);
        auto npi = n*M_PI;
        auto cx = cos(npi*x);
        auto st = sin(2*npi*t);
        return factor*st*cx*cx;
    }

    AnalyticOscillon1p1_FourierImpl::AnalyticOscillon1p1_FourierImpl(
            const AnalyticOscillon1p1_FourierImpl::Parameters &parameters, Int n_max)
            : parameters(parameters)
            , n_max(n_max)
            , f(phi_n){

        auto &p = parameters;
        gamma = 1./ sqrt(1.-p.u*p.u);
        x_min = (p.x0-.5*p.l/gamma);
        x_max = (p.x0+.5*p.l/gamma);

    }

    DevFloat AnalyticOscillon1p1_FourierImpl::operator()(Real2D r) const {
        fix &p = parameters;

        fix ga = gamma*p.alpha;
        fix uga = p.u*ga;

        fix x = r.x-uga, t = r.y-ga;
        fix x_mov = (x+p.u*t);
        if(x_mov<=x_min || x_mov>=x_max) return 0.0;

        constexpr auto norm_factor = 2./(M_PI*M_PI*M_PI);

        fix u = p.u;
        fix x0 = p.x0;
        // fix ut = u*t;
        // fix x0ut = x0+ut;
        fix λ = p.l;
        // fix γ_inv = sqrt(1-u*u);

        fix xp = gamma*(x-x0+u*t)/λ;
        fix tp = gamma*(t+u*(x-x0))/λ;

        auto sum = .0;
        auto sign = 1;
        for(int n=1; n<=n_max; n+=2) {
            sum += sign*f(n, tp, xp);
            sign *= -1;
        }

        return λ*λ*norm_factor*sum;
    }

    void AnalyticOscillon1p1_FourierImpl::setBit(Slab::Math::RtoR::AnalyticOscillon::Bit bit) {
        switch (bit) {
            case RtoR::AnalyticOscillon::phi:
                f = phi_n;
                return;
            case RtoR::AnalyticOscillon::dPhiDt:
                f = ddtphi_n;
                return;
        }

        NOT_IMPLEMENTED
    }

    void AnalyticOscillon1p1_FourierImpl::setParams(AnalyticOscillon1p1_FourierImpl::Parameters parameters1) {
        parameters = parameters1;
    }

    auto AnalyticOscillon1p1_FourierImpl::Clone() const -> TPointer<Base::FunctionT<Real2D, DevFloat>::Type> {
        auto clone = New<AnalyticOscillon1p1_FourierImpl>(parameters, n_max);
        clone->f = this->f;

        return clone;
    }

} // Slab::Math::R2toR