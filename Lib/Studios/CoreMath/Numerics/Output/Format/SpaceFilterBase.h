//
// Created by joao on 14/10/2019.
//

#ifndef V_SHAPE_SPACEFILTERBASE_H
#define V_SHAPE_SPACEFILTERBASE_H

#include "Studios/CoreMath/Numerics/Output/Util/OutputPacket.h"

#include <Studios/CoreMath/Function/Space/DiscreteSpace.h>

class SpaceFilterBase {
public:
    /*!
     * The filter operator.
     * @param space input DiscreteSpace to be filtered.
     * @return a filtered copy of `space`  parameter.
     */
    virtual auto operator()(const OutputPacket &outputInfo) -> DiscreteSpacePair = 0;

    virtual auto getOutputDim() const -> DimensionMetaData = 0;
};

typedef SpaceFilterBase BypassFilter;


#endif //V_SHAPE_SPACEFILTERBASE_H