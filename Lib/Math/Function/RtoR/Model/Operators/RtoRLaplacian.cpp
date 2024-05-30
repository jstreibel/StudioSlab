//
// Created by joao on 5/29/24.
//

#include "RtoRLaplacian.h"

namespace Slab::Math::RtoR {

    Base::DiscreteFunction<Real, Real> &RtoRLaplacian::operateAndStoreResult(Base::DiscreteFunction<Real, Real> &output,
                                                                       const Base::DiscreteFunction<Real, Real> &funky) const {
        auto& outputty = static_cast<DiscreteFunction&> (output);
        auto& dFunky = static_cast<const DiscreteFunction&> (funky);

        return dFunky.Laplacian(outputty);
    }


    typedef Operator<Base::DiscreteFunction<Real,Real>> Oppy;
    typedef Operator<DiscreteFunction> Oppyo;
    void testy(DiscreteFunction &a, const DiscreteFunction &b, const Oppy &O, const Oppyo &L) {
        auto c = O*b;
        auto z = L*b;

        a = c;
    }


} // Slab::Math::RtoR