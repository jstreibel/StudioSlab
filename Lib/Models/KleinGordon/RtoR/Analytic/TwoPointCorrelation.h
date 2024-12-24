//
// Created by joao on 10/6/24.
//

#ifndef STUDIOSLAB_TWOPOINTCORRELATION_H
#define STUDIOSLAB_TWOPOINTCORRELATION_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Models::KGRtoR {

    class CorrelationDecay : public Math::RtoR::Function {
        Real c0;
        Real invξ;
        Real β;
        Real u0;
    public:
        Real getU0() const;

        void setU0(Real u0_new);

    public:
        enum Nature { Exponential, Power };
    private:
        Nature nature = Power;
    public:

        Real get_c0() const;
        void set_c0(Real c0);

        Real getξ() const;
        void setξ(Real invξ);

        Real getβ() const;
        void setβ(Real β);

        Nature getNature() const;
        void setNature(CorrelationDecay::Nature nat);


        explicit CorrelationDecay(Real c0=1, Real ξ=1, Real β=2, Real u0=0);

        Real operator()(Real u) const override;
    };




    class TwoPointCorrelation : public Math::RtoR::Function {
        CorrelationDecay c;
        Real λ;
        Count n_max;
        Real ϕ_0 = .0;
    public:
        Real getΦ0() const;

        void setΦ0(Real φ0);

    public:
        Real get_c0() const;
        void set_c0(Real c0);

        CorrelationDecay::Nature getNature() const;
        void setNature(CorrelationDecay::Nature);

        Real getλ() const;
        void setλ(Real λ);

        Real getξ() const;
        void setξ(Real ξ);

        Real getβ() const;
        void setβ(Real β);

        Count getNMax() const;
        void setNMax(Count nMax);

    public:
        explicit TwoPointCorrelation(Real c0=1, Real λ=1, Real ξ=4, Real β=2, Count n=15);

        Real operator()(Real u) const override;
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_TWOPOINTCORRELATION_H
