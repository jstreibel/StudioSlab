//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACECPU_H
#define V_SHAPE_DISCRETESPACECPU_H

#include "DiscreteSpace.h"


namespace Slab::Math {

    class DiscreteSpaceCPU : public DiscreteSpace {
    public:
        explicit DiscreteSpaceCPU(DimensionMetaData dim);

        [[nodiscard]] Device GetDevice() const override { return CPU; };

        auto Add(const DiscreteSpace &toi) -> DiscreteSpace & override;

        auto StoreAddition(const DiscreteSpace &space1, const DiscreteSpace &space2) -> DiscreteSpace & override;

        auto Subtract(const DiscreteSpace &toi) -> DiscreteSpace & override;

        auto StoreSubtraction(const DiscreteSpace &space1, const DiscreteSpace &space2) -> DiscreteSpace & override;

        auto Multiply(floatt a) -> DiscreteSpace & override;

        auto StoreScalarMultiplication(const DiscreteSpace &aoi1, const DevFloat a) -> DiscreteSpace & override;

        auto setToValue(const DiscreteSpace &param) -> void override;
    };

}

#endif //V_SHAPE_DISCRETESPACECPU_H
