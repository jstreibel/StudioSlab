//
// Created by joao on 14/10/2019.
//

#include "ResolutionReductionFilter.h"

#include "Math/VectorSpace/Impl/DiscreteSpace.h"
#include "Math/VectorSpace/Impl/DiscreteSpaceCPU.h"


namespace Slab::Math {

    ResolutionReductionFilter::ResolutionReductionFilter(DimensionMetaData newDim) : newDim(std::move(newDim)) {

    }

    auto ResolutionReductionFilter::operator()(const FOutputPacket &outputInfo) -> DiscreteSpacePair {
        NOT_IMPLEMENTED

        /*
        DiscreteSpacePair inputSpacePair = outputInfo.getSpaceData();
        const DiscreteSpace *phi = inputSpacePair.first;
        const DiscreteSpace *dPhi = inputSpacePair.second;

        // NOTE meta-operators came to mind

        DiscreteSpaceCPU *newPhi = nullptr;
        DiscreteSpaceCPU *newDPhi = nullptr;
        if (newDim.getNDim() == 1) {
            newPhi = phi->hostCopy(newDim.getN(0));
            newDPhi = dPhi->hostCopy(newDim.getN(0));
        } else {
            throw "Higher res filter not implemented.";
        }

        return {newPhi, newDPhi};
         */
    }

    auto ResolutionReductionFilter::getOutputDim() const -> DimensionMetaData {
        return newDim;
    }


}