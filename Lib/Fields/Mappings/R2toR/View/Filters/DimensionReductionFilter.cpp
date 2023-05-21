//

#include "DimensionReductionFilter.h"

#include <Phys/Space/Impl/DiscreteSpaceCPU.h>

#include "Fields/Mappings/R2toR/Model/R2toRFunctionArbitrary.h"
#include "Fields/Mappings/R2toR/Model/FieldState.h"


//
// Created by joao on 15/10/2019.
DimensionReductionFilter::DimensionReductionFilter(PosInt resolution, RtoR2::StraightLine alongLine)
    : line(alongLine), N(resolution) {

}

DiscreteSpacePair DimensionReductionFilter::operator()(const OutputPacket &outputInfo) {
    const DiscreteSpace &phiSpace = *outputInfo.getSpaceData().first;
    const DiscreteSpace &dPhiSpace = *outputInfo.getSpaceData().second;

    if(phiSpace.getDim().getNDim() != 2) throw "Is rong.";

    R2toR::FunctionArbitrary &f = outputInfo.getFieldData<R2toR::FieldState>()->getPhi();

    const R2toR::Domain domain = f.getDomain();
    const auto L = domain.getLx();
    const Real h = L/N;
    const auto ds = line.getDeltaS();
    const auto sMin = line.getSMin();
    DiscreteSpaceCPU *newPhi = new DiscreteSpaceCPU(getOutputDim(), h);

    for(PosInt i=0; i<N; i++){
        const Real s = sMin + ds*Real(i)/N; // parametro da reta.
        newPhi->getX()[i] = f(line(s));
    }

    return {newPhi, nullptr};
}

DimensionMetaData DimensionReductionFilter::getOutputDim() const {
    return DimensionMetaData({N});
}
