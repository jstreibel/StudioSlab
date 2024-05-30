//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARY_H
#define V_SHAPE_RTORFUNCTIONARBITRARY_H

#include "RtoRFunction.h"

#include "Math/Function/DiscreteFunction.h"

//#include <Core/Controller/Interface/CommonParameters.h>
#include "Utils/Types.h"


namespace Slab::Math::RtoR {

    class DiscreteFunction : public Base::DiscreteFunction<Real, Real> {
    public:
        using NumericAlgebra<Base::DiscreteFunction<Real, Real>>::operator=;

        enum LaplacianType {
            Standard1D_FixedBorder,
            Standard1D_PeriodicBorder,
            RadialSymmetry2D,
        };

        DiscreteFunction(const DiscreteFunction &toCopy);
        DiscreteFunction(UInt N, Real xMin, Real xMax, device dev, LaplacianType laplacianType = LaplacianType::Standard1D_FixedBorder);

        virtual DiscreteFunction &Laplacian(DiscreteFunction &outFunc) const = 0;
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

    DefinePointer(DiscreteFunction)
}

#endif //V_SHAPE_RTORFUNCTIONARBITRARY_H
