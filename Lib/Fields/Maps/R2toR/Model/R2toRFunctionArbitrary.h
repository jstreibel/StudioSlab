//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARY_H
#define V_SHAPE_R2TORFUNCTIONARBITRARY_H

#include "R2toRFunction.h"
#include "Studios/Util/ArithmeticOpsInterface.h"
#include "Studios/MathTools/FunctionArbitrary.h"
#include "Util/EXPOSE_BASE.h"
#include "Studios/Controller/Interface/CommonParameters.h"


namespace R2toR {
    class Domain {
    public:
        Real getLx() const { return xMax-xMin; }
        Real getLy() const { return yMax-yMin; }
        const Real xMin, xMax, yMin, yMax;
    };

    class FunctionArbitrary : public Base::FunctionArbitrary<Real2D,Real> {
    EXPOSE_BASE(Function)
        typedef Base::FunctionArbitrary <Real2D, Real> FunctionArbitraryBase;

    public:
        FunctionArbitrary(PosInt N, PosInt M, Real xMin, Real yMin, Real h, device dev);

        Real operator()(Real2D x) const override;

        Real diff(int n, Real2D x) const override;

        FunctionPtr diff(int n) const override;

    public:
        virtual FunctionArbitrary &Laplacian(FunctionArbitrary &outFunc) const = 0;
        virtual Real At(PosInt n, PosInt m) const = 0;
        virtual Real &At(PosInt n, PosInt m) = 0;

        PosInt getN() const;
        PosInt getM() const;

        Domain getDomain() const;

    protected:
        const PosInt N;
        const PosInt M;
        const Real xMin, xMax, yMin, yMax, h;
    };
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARY_H
