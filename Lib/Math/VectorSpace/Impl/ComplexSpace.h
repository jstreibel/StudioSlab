//
// Created by joao on 10/05/24.
//

#ifndef STUDIOSLAB_COMPLEXSPACE_H
#define STUDIOSLAB_COMPLEXSPACE_H

#include "Math/VectorSpace/Algebra/NumericAlgebra.h"
#include "Math/VectorSpace/DimensionMetaData.h"

namespace Slab::Math {

    class DiscreteComplexSpace : public NumericAlgebra<DiscreteComplexSpace> {
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
        auto StoreScalarMultiplication (const DiscreteComplexSpace &aoi1,   const DevFloat a) -> DiscreteComplexSpace & override;

        auto setToValue          (const DiscreteComplexSpace &param)                -> void;


    };

} // Math

#endif //STUDIOSLAB_COMPLEXSPACE_H
