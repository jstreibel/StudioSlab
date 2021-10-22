//
// Created by joao on 15/10/2019.
//

#ifndef V_SHAPE_DIMENSIONREDUCTIONFILTER_H
#define V_SHAPE_DIMENSIONREDUCTIONFILTER_H

#include "View/SpaceFilter/SpaceFilterBase.h"
#include "Apps/Simulations/Maps/RtoR2/StraightLine.h"

class DimensionReductionFilter : public SpaceFilterBase {
    /**
     * This classes reduces the dimension of a 2d space to 1d along a determined line
     * */
public:
    DimensionReductionFilter(PosInt resolution, RtoR2::StraightLine alongLine);

    DiscreteSpacePair operator()(const OutputPacket &outputInfo) override;

    DimensionMetaData getOutputDim() const override;

private:
    const RtoR2::StraightLine line;
    const PosInt N;
};


#endif //V_SHAPE_DIMENSIONREDUCTIONFILTER_H
