//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARY_H
#define V_SHAPE_RTORFUNCTIONARBITRARY_H

#include "RtoRFunction.h"

#include <Phys/Function/ArbitraryFunction.h>

#include <Studios/Controller/Interface/CommonParameters.h>
#include <Common/Typedefs.h>


namespace RtoR {

    class ArbitraryFunction : public Base::ArbitraryFunction<Real, Real> {
    public:
        enum LaplacianType {
            Standard1D,
            RadialSymmetry2D,
        };

    public:
        ArbitraryFunction(const ArbitraryFunction &toCopy);
        ArbitraryFunction(PosInt N, Real xMin, Real xMax, device dev, LaplacianType laplacianType = LaplacianType::Standard1D);

    public:
        virtual ArbitraryFunction &Laplacian(ArbitraryFunction &outFunc) const = 0;

    public:
        Real mapIntToPos(PosInt i) const;
        PosInt mapPosToInt(Real x) const;

        Real operator()(Real x) const override;

        const PosInt N;
        const Real xMin;
        const Real xMax;
    protected:
        LaplacianType laplacianType;
    };

}

#endif //V_SHAPE_RTORFUNCTIONARBITRARY_H
