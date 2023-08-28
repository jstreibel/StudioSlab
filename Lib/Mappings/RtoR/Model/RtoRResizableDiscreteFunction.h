//
// Created by joao on 16/04/2021.
//

#ifndef V_SHAPE_RTORFUNCARBRESIZABLE_H
#define V_SHAPE_RTORFUNCARBRESIZABLE_H


#include "Math/Function/DiscreteFunction.h"
#include "RtoRFunction.h"

namespace RtoR {
    class ResizableDiscreteFunction : public Base::FunctionT<Real, Real> {
        Real yMin=1, yMax=-1;
    public:
        auto getYMin() const -> Real;
        auto getYMax() const -> Real;


        std::vector<Real> X;
        Real xMin, xMax;

        ResizableDiscreteFunction() : xMin(0), xMax(0), X(0) {};
        ResizableDiscreteFunction(Real xMin, Real xMax);

        auto operator()(Real x) const -> Real override;
        auto operator=(const ResizableDiscreteFunction &func) -> ResizableDiscreteFunction& {
            X = func.X;
            xMin = func.xMin;
            xMax = func.xMax;

            return *this;
        }

        void insertBack(Real y);

    };
}


#endif //V_SHAPE_RTORFUNCARBRESIZABLE_H
