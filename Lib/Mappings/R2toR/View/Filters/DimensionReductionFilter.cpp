//

#include "DimensionReductionFilter.h"

#include "Phys/Space/Impl/DiscreteSpaceCPU.h"

#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"
#include "Mappings/R2toR/Model/EquationState.h"


//
// Created by joao on 15/10/2019.
DimensionReductionFilter::DimensionReductionFilter(PosInt resolution, RtoR2::StraightLine alongLine)
    : line(alongLine), N(resolution) {

}

DiscreteSpacePair DimensionReductionFilter::operator()(const OutputPacket &outputInfo) {
    const DiscreteSpace &phiSpace = *outputInfo.getSpaceData().first;
    const DiscreteSpace &dPhiSpace = *outputInfo.getSpaceData().second;

    if(phiSpace.getDim().getNDim() != 2) throw "Is rong.";

    R2toR::DiscreteFunction &f = outputInfo.getEqStateData<R2toR::EquationState>()->getPhi();

    const R2toR::Domain domain = f.getDomain();
    const auto L = domain.getLx();
    const Real h = L/N;
    const auto ds = line.getDeltaS();
    const auto sMin = line.getSMin();
    DiscreteSpaceCPU *newPhi = new DiscreteSpaceCPU(getOutputDim(), h);

    for(PosInt i=0; i<N; i++){
        const Real s = sMin + ds*Real(i)/N; // parametro da reta.
        newPhi->getHostData()[i] = f(line(s));
    }

    return {newPhi, nullptr};
}

DimensionMetaData DimensionReductionFilter::getOutputDim() const {
    return DimensionMetaData({N});
}
