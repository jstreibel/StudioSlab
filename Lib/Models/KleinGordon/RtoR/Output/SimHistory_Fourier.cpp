//
// Created by joao on 12/09/23.
//

#include "SimHistory_Fourier.h"

#define nConfig (simConfig.numericConfig)
#define NDFTModes (nConfig.getN()/2+1)
#define kMaxDFT (NDFTModes*2*M_PI/nConfig.getL())


SimHistory_FourierTransform::SimHistory_FourierTransform(const Core::Simulation::SimulationConfig &simConfig, Resolution n, Real kMin, Real kMax)
: SimHistory(simConfig, n, kMin, kMax-kMin, "FTSimHistory")
, fourierModes(nullptr, nConfig.getxMin(), nConfig.getL(), nConfig.getN()/10) {}


Real SimHistory_FourierTransform::filter(Real x, const RtoR::EquationState &input) {
    return abs(fourierModes(x));
}


void SimHistory_FourierTransform::handleOutput(const OutputPacket &packet) {
    IN state = *packet.getEqStateData<RtoR::EquationState>();

    fourierModes.setBaseFunction(DummyPtr(state.getPhi()));

    SimHistory::handleOutput(packet);
}


SimHistory_DFT::SimHistory_DFT(const Core::Simulation::SimulationConfig &simConfig)
: SimHistory(simConfig, NDFTModes, 0, kMaxDFT)
, dft(NDFTModes, 0.0, kMaxDFT)
{

}

Real SimHistory_DFT::filter(Real x, const RtoR::EquationState &input) {
    return dft(x);
}

void SimHistory_DFT::handleOutput(const OutputPacket &packet) {
    IN phi = packet.getEqStateData<RtoR::EquationState>()->getPhi();

    fix pts = RtoR::FourierTransform::Compute(phi).getAbs()->getPoints();

    OUT dftSpace = dft.getSpace().getHostData(true);
    for(auto i=0; i<pts.size(); ++i) {
        fix &pt = pts[i];

        dftSpace[i] = pt.y;
    }

    SimHistory::handleOutput(packet);
}
