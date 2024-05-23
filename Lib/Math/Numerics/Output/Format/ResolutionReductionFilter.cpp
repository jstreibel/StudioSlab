//
// Created by joao on 14/10/2019.
//

#include "ResolutionReductionFilter.h"

#include "Math/Space/Impl/DiscreteSpace.h"
#include "Math/Space/Impl/DiscreteSpaceCPU.h"


namespace Slab::Math {

    ResolutionReductionFilter::ResolutionReductionFilter(DimensionMetaData newDim) : newDim(std::move(newDim)) {

    }

    auto ResolutionReductionFilter::operator()(const OutputPacket &outputInfo) -> DiscreteSpacePair {
        DiscreteSpacePair inputSpacePair = outputInfo.getSpaceData();
        const DiscreteSpace *phi = inputSpacePair.first;
        const DiscreteSpace *dPhi = inputSpacePair.second;

        // TODO came to my mind: meta-operators?

        DiscreteSpaceCPU *newPhi = nullptr;
        DiscreteSpaceCPU *newDPhi = nullptr;
        if (newDim.getNDim() == 1) {
            newPhi = phi->hostCopy(newDim.getN(0));
            newDPhi = dPhi->hostCopy(newDim.getN(0));
        } else {
            throw "Higher res filter not implemented.";
        }

        return {newPhi, newDPhi};
    }

    auto ResolutionReductionFilter::getOutputDim(const Real L) const -> DimensionMetaData {
        return newDim;
    }


}