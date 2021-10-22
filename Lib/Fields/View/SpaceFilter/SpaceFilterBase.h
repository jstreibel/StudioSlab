//
// Created by joao on 14/10/2019.
//

#ifndef V_SHAPE_SPACEFILTERBASE_H
#define V_SHAPE_SPACEFILTERBASE_H

#include "Model/DiscreteSpace/DiscreteSpace.h"
#include "View/Util/OutputPacket.h"

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
