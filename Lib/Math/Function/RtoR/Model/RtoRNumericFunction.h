//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARY_H
#define V_SHAPE_RTORFUNCTIONARBITRARY_H

#include "RtoRFunction.h"

#include "Math/Function/NumericFunction.h"

//#include <Core/Controller/Interface/CommonParameters.h>
#include "Utils/Types.h"


namespace Slab::Math::RtoR {

    class NumericFunction : public Base::NumericFunction<Real, Real> {
    public:
        using NumericAlgebra<Base::NumericFunction<Real, Real>>::operator=;

        enum LaplacianType {
            Standard1D_FixedBorder,
            Standard1D_PeriodicBorder,
            RadialSymmetry2D,
        };

        NumericFunction(const NumericFunction &toCopy);
        NumericFunction(UInt N, Real xMin, Real xMax, device dev, LaplacianType laplacianType = LaplacianType::Standard1D_FixedBorder);

        virtual NumericFunction &Laplacian(NumericFunction &outFunc) const = 0;
        LaplacianType getLaplacianType() const { return laplacianType; }

        Real mapIntToPos(UInt i) const;
        UInt mapPosToInt(Real x) const;

        Real operator()(Real x) const override;

        const UInt N;
        const Real xMin;
        const Real xMax;

    protected:
        LaplacianType laplacianType;
    };

    DefinePointer(NumericFunction)
}

#endif //V_SHAPE_RTORFUNCTIONARBITRARY_H
