//
// Created by joao on 14/10/2019.
//

#include "ResolutionReductionFilter.h"

#include <utility>
#include "Model/DiscreteSpace/DiscreteSpaceCPU.h"

ResolutionReductionFilter::ResolutionReductionFilter(DimensionMetaData newDim) : newDim(std::move(newDim)) {

}

auto ResolutionReductionFilter::operator()(const OutputPacket &outputInfo) -> DiscreteSpacePair {
    DiscreteSpacePair inputSpacePair = outputInfo.getSpaceData();
    const DiscreteSpace *phi = inputSpacePair.first;
    const DiscreteSpace *dPhi = inputSpacePair.second;

    DiscreteSpaceCPU *newPhi = nullptr;
    DiscreteSpaceCPU *newDPhi = nullptr;
    if(newDim.getNDim() == 1) {
        newPhi = phi->hostCopy(newDim[0]);
        newDPhi = dPhi->hostCopy(newDim[0]);
    }
    else{
        throw "Higher res filter not implemented.";
    }

    return {newPhi, newDPhi};
}

auto ResolutionReductionFilter::getOutputDim() const -> DimensionMetaData {
    return newDim;
}
