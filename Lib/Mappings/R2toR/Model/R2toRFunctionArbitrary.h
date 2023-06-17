//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARY_H
#define V_SHAPE_R2TORFUNCTIONARBITRARY_H

#include "Phys/Function/ArbitraryFunction.h"

#include "R2toRFunction.h"

namespace R2toR {
    class Domain {
    public:
        Real getLx() const { return xMax-xMin; }
        Real getLy() const { return yMax-yMin; }
        const Real xMin, xMax, yMin, yMax;
    };

    class FunctionArbitrary : public Base::ArbitraryFunction<Real2D,Real> {
    protected:
        const PosInt N;
        const PosInt M;
        const Real xMin, xMax, yMin, yMax, h;

    public:
        typedef Base::ArbitraryFunction <Real2D, Real> FunctionArbitraryBase;

        FunctionArbitrary(PosInt N, PosInt M, Real xMin, Real yMin, Real h, device dev);

        Real operator()(Real2D x) const override;
        void operator=(const MyBase &func);

        Real diff(int n, Real2D x) const override;

        MyBase::Ptr diff(int n) const override;

        virtual FunctionArbitrary &Laplacian(FunctionArbitrary &outFunc) const = 0;
        virtual Real At(PosInt n, PosInt m) const = 0;
        virtual Real &At(PosInt n, PosInt m) = 0;

        PosInt getN() const;
        PosInt getM() const;

        Domain getDomain() const;

    };
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARY_H
