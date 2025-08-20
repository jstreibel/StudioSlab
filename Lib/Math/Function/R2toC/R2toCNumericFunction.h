//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_R2TOCNumericFunction_H
#define STUDIOSLAB_R2TOCNumericFunction_H

#include "Math/Function/Function.h"
#include "Math/VectorSpace/Impl/ComplexSpace.h"
#include "Math/Data/Data.h"

namespace Slab::Math::R2toC {

    class NumericFunction : public Base::FunctionT<Real2D, Complex>, public Data {
        ComplexArray data;

    public:

        const DevFloat x0, y0;
        const DevFloat Lx, Ly;
        const Resolution N, M;

        NumericFunction(Resolution N, Resolution M, DevFloat x0, DevFloat y0, DevFloat Lx, DevFloat Ly);
        ~NumericFunction() = default;

        float get_data_size_MiB() override;

        Complex operator()(Real2D x) const override;
        Complex &At(Real2D x);
        Complex &At(int i, int j);

        auto getData() const -> const ComplexArray&;
        auto getData() -> ComplexArray&;

        DataType get_data_type() const override;
    };

    DefinePointers(NumericFunction)

} // R2toC

#endif //STUDIOSLAB_R2TOCNumericFunction_H
