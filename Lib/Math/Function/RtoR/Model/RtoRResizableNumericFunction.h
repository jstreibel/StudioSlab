//
// Created by joao on 16/04/2021.
//

#ifndef V_SHAPE_RTORFUNCARBRESIZABLE_H
#define V_SHAPE_RTORFUNCARBRESIZABLE_H


#include "Math/Function/NumericFunction.h"
#include "RtoRFunction.h"

namespace Slab::Math::RtoR {

    class ResizableNumericFunction : public Base::FunctionT<DevFloat, DevFloat> {
        DevFloat yMin=1, yMax=-1;
    public:
        auto getYMin() const -> DevFloat;
        auto getYMax() const -> DevFloat;


        Vector<DevFloat> X;
        DevFloat xMin, xMax;

        ResizableNumericFunction() : xMin(0), xMax(0), X(0) {};
        ResizableNumericFunction(DevFloat xMin, DevFloat xMax);

        auto operator()(DevFloat x) const -> DevFloat override;
        auto operator=(const ResizableNumericFunction &func) -> ResizableNumericFunction& {
            X = func.X;
            xMin = func.xMin;
            xMax = func.xMax;

            return *this;
        }

        void insertBack(DevFloat y);

    };
}


#endif //V_SHAPE_RTORFUNCARBRESIZABLE_H
