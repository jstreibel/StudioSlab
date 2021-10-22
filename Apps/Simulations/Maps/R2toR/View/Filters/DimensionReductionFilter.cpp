//

#include "DimensionReductionFilter.h"
#include "Model/DiscreteSpace/DiscreteSpaceCPU.h"
#include "Apps/Simulations/Maps/R2toR/Model/R2toRFunctionArbitrary.h"
#include "Apps/Simulations/Maps/R2toR/Model/FieldState.h"



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
    const Real h = domain.getLx()/N;
    DiscreteSpaceCPU *newPhi = new DiscreteSpaceCPU(getOutputDim(), h);

    for(PosInt i=0; i<N; i++){
        const Real s = Real(i)/N; // parametro da reta.
        newPhi->getX()[i] = f(line(s));
    }

    return {newPhi, nullptr};
}

DimensionMetaData DimensionReductionFilter::getOutputDim() const {
    return DimensionMetaData({N});
}
