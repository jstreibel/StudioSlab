//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARY_H
#define V_SHAPE_RTORFUNCTIONARBITRARY_H

#include "Math/Function/NumericFunction.h"

#include "Utils/Types.h"


namespace Slab::Math::RtoR {

    class NumericFunction : public Base::NumericFunction<DevFloat, DevFloat> {
    public:
        using NumericAlgebra<Base::NumericFunction<DevFloat, DevFloat>>::operator=;

        enum LaplacianType {
            Standard1D_FixedBorder,
            Standard1D_PeriodicBorder,
            RadialSymmetry2D,
        };

        NumericFunction(const NumericFunction &toCopy);
        NumericFunction(UInt N, DevFloat xMin, DevFloat xMax, Device dev, LaplacianType laplacianType = LaplacianType::Standard1D_FixedBorder);

        virtual NumericFunction &Laplacian(NumericFunction &outFunc) const = 0;

        LaplacianType
        getLaplacianType() const { return laplacianType; }

        DevFloat mapIntToPos(UInt i) const;
        UInt mapPosToInt(DevFloat x) const;

        DevFloat operator()(DevFloat x) const override;

        const UInt N;
        const DevFloat xMin;
        const DevFloat xMax;

    protected:
        LaplacianType laplacianType;
    };

    DefinePointers(NumericFunction)
}

#endif //V_SHAPE_RTORFUNCTIONARBITRARY_H
