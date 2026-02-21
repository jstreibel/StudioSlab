//
// Created by joao on 14/10/2019.
//

#ifndef V_SHAPE_SPACEFILTERBASE_H
#define V_SHAPE_SPACEFILTERBASE_H

#include "Math/Numerics/ODE/Output/Util/OutputPacket.h"

#include "Math/VectorSpace/Impl/DiscreteSpace.h"


namespace Slab::Math {

    class FSpaceFilterBase {
    public:
        virtual ~FSpaceFilterBase() = default;

        /*!
         * The filter operator.
         * @param space input DiscreteSpace to be filtered.
         * @return a filtered copy of `space`  parameter.
         */
        virtual auto operator()(const FOutputPacket &outputInfo) -> DiscreteSpacePair = 0;

        virtual auto getOutputDim() const -> DimensionMetaData = 0;
    };

    using FBypassFilter = FSpaceFilterBase;
    using SpaceFilterBase [[deprecated("Use FSpaceFilterBase")]] = FSpaceFilterBase;
    using BypassFilter [[deprecated("Use FBypassFilter")]] = FBypassFilter;


}

#endif //V_SHAPE_SPACEFILTERBASE_H
