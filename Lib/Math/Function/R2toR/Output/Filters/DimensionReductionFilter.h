//
// Created by joao on 15/10/2019.
//

#ifndef V_SHAPE_DIMENSIONREDUCTIONFILTER_H
#define V_SHAPE_DIMENSIONREDUCTIONFILTER_H


#include "Math/Numerics/ODE/Output/Format/SpaceFilterBase.h"
#include "Math/Function/RtoR2/StraightLine.h"


namespace Slab::Math::R2toR {

    class DimensionReductionFilter : public SpaceFilterBase {
        /**
         * This classes reduces the dimension of a 2d space to 1d along a determined line
         * */
    public:
        DimensionReductionFilter(UInt resolution, RtoR2::StraightLine alongLine, Real L);

        DiscreteSpacePair operator()(const OutputPacket &outputInfo) override;

        DimensionMetaData getOutputDim() const override;

    private:
        const Real L;
        const RtoR2::StraightLine line;
        const UInt N_low;
    };


}

#endif //V_SHAPE_DIMENSIONREDUCTIONFILTER_H
