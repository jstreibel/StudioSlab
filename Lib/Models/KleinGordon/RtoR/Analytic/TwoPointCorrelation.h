//
// Created by joao on 10/6/24.
//

#ifndef STUDIOSLAB_TWOPOINTCORRELATION_H
#define STUDIOSLAB_TWOPOINTCORRELATION_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Models::KGRtoR {

    class CorrelationDecay : public Math::RtoR::Function {
        DevFloat c0;
        DevFloat invξ;
        DevFloat β;
        DevFloat u0;
    public:
        DevFloat getU0() const;

        void setU0(DevFloat u0_new);

    public:
        enum Nature { Exponential, Power };
    private:
        Nature nature = Power;
    public:

        DevFloat get_c0() const;
        void set_c0(DevFloat c0);

        DevFloat getξ() const;
        void setξ(DevFloat invξ);

        DevFloat getβ() const;
        void setβ(DevFloat β);

        Nature getNature() const;
        void setNature(CorrelationDecay::Nature nat);


        explicit CorrelationDecay(DevFloat c0=1, DevFloat ξ=1, DevFloat β=2, DevFloat u0=0);

        DevFloat operator()(DevFloat u) const override;
    };




    class TwoPointCorrelation : public Math::RtoR::Function {
        CorrelationDecay c;
        DevFloat λ;
        CountType n_max;
        DevFloat ϕ_0 = .0;
    public:
        DevFloat getΦ0() const;

        void setΦ0(DevFloat φ0);

    public:
        DevFloat get_c0() const;
        void set_c0(DevFloat c0);

        CorrelationDecay::Nature getNature() const;
        void setNature(CorrelationDecay::Nature);

        DevFloat getλ() const;
        void setλ(DevFloat λ);

        DevFloat getξ() const;
        void setξ(DevFloat ξ);

        DevFloat getβ() const;
        void setβ(DevFloat β);

        CountType getNMax() const;
        void setNMax(CountType nMax);

    public:
        explicit TwoPointCorrelation(DevFloat c0=1, DevFloat λ=1, DevFloat ξ=4, DevFloat β=2, CountType n=15);

        DevFloat operator()(DevFloat u) const override;
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_TWOPOINTCORRELATION_H
