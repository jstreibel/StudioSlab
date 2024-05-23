//
// Created by joao on 10/05/24.
//

#ifndef STUDIOSLAB_COMPLEXSPACE_H
#define STUDIOSLAB_COMPLEXSPACE_H

#include "ArithmeticOperationInterface.h"
#include "Math/Space/DimensionMetaData.h"

namespace Slab::Math {

    class DiscreteComplexSpace : public ArithmeticOpsInterface<DiscreteComplexSpace> {
    protected:
        const DimensionMetaData dimensionMetaData;
        ComplexArray data;

    public:
        explicit DiscreteComplexSpace(const DimensionMetaData& dim);

        const DimensionMetaData& getMetaData() const { return dimensionMetaData; }

        UInt getTotalDiscreteSites() const;

        auto getData( ) const -> const ComplexArray&;
        auto getData( )       ->       ComplexArray&;

    public:
        auto Add                 (const DiscreteComplexSpace &toi)                                        -> DiscreteComplexSpace & override;
        auto StoreAddition       (const DiscreteComplexSpace &space1, const DiscreteComplexSpace &space2) -> DiscreteComplexSpace & override;

        auto Subtract            (const DiscreteComplexSpace &toi)                                        -> DiscreteComplexSpace & override;
        auto StoreSubtraction    (const DiscreteComplexSpace &space1, const DiscreteComplexSpace &space2) -> DiscreteComplexSpace & override;

        auto Multiply            (floatt a)                                         -> DiscreteComplexSpace & override;
        auto StoreMultiplication (const DiscreteComplexSpace &aoi1,   const Real a) -> DiscreteComplexSpace & override;

        auto setToValue          (const DiscreteComplexSpace &param)                -> void;

        DiscreteComplexSpace &operator*=(Real a) override;


    };

} // Math

#endif //STUDIOSLAB_COMPLEXSPACE_H
