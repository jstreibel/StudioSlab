//
// Created by joao on 14/10/2019.
//

#ifndef V_SHAPE_RESOLUTIONREDUCTIONFILTER_H
#define V_SHAPE_RESOLUTIONREDUCTIONFILTER_H

#include "SpaceFilterBase.h"


namespace Slab::Math {

/*! A discrete space filter that reduces resolution and returns a CPU- (host) located discrete space. */
    class ResolutionReductionFilter : public SpaceFilterBase {
    public:
        explicit ResolutionReductionFilter(DimensionMetaData newDim);

        DiscreteSpacePair operator()(const FOutputPacket &outputInfo) override;

        auto getOutputDim() const -> DimensionMetaData override;

    private:
        DimensionMetaData newDim;
    };


}

#endif //V_SHAPE_RESOLUTIONREDUCTIONFILTER_H
