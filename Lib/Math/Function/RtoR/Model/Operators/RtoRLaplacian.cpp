//
// Created by joao on 5/29/24.
//

#include "RtoRLaplacian.h"

namespace Slab::Math::RtoR {

    Base::NumericFunction<Real, Real> &RtoRLaplacian::operateAndStoreResult(Base::NumericFunction<Real, Real> &output,
                                                                            const Base::NumericFunction<Real, Real> &funky) const {
        auto& outputty = static_cast<NumericFunction&> (output);
        auto& dFunky = static_cast<const NumericFunction&> (funky);

        return dFunky.Laplacian(outputty);
    }


    typedef Operator<Base::NumericFunction<Real,Real>> Oppy;
    typedef Operator<NumericFunction> Oppyo;
    void testy(NumericFunction &a, const NumericFunction &b, const Oppy &O, const Oppyo &L) {
        auto c = O*b;
        auto z = L*b;

        a = c;
    }


} // Slab::Math::RtoR