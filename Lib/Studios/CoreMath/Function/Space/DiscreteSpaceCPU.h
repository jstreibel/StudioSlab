//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACECPU_H
#define V_SHAPE_DISCRETESPACECPU_H


#include "DiscreteSpace.h"

class DiscreteSpaceCPU : public DiscreteSpace {
public:
    DiscreteSpaceCPU(DimensionMetaData dim, Real h);

    auto Add(const DiscreteSpace &toi) -> DiscreteSpace & override;

    auto StoreAddition(const DiscreteSpace &space1, const DiscreteSpace &space2) -> DiscreteSpace & override;

    auto StoreSubtraction(const DiscreteSpace &space1, const DiscreteSpace &space2) -> DiscreteSpace & override;

    auto Multiply(floatt a) -> DiscreteSpace & override;

    void setToValue(const DiscreteSpace &param) override;
};


#endif //V_SHAPE_DISCRETESPACECPU_H