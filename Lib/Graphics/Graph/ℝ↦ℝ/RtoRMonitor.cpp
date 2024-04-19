//
//
// Created by joao on 23/09/2019.


#include "RtoRMonitor.h"

#include <utility>

#include "Models/KleinGordon/KGSolver.h"
#include "Maps/RtoR/Calc/FourierTransform.h"
#include "RtoRRealtimePanel.h"


RtoR::Monitor::Monitor(const NumericConfig &params, KGEnergy &hamiltonian,
                       const Real phiMin, const Real phiMax, const Str& name, bool showEnergyHistoryAsDensities)
: ::Graphics::OpenGLMonitor(params, Str("ℝ↦ℝ ") + name)
, hamiltonian(hamiltonian)
{
    auto currStyle = Math::StylesManager::GetCurrent();

    currentDataView = new RtoR::RealtimePanel(params, hamiltonian, stats,
                                              phiMin, phiMax, showEnergyHistoryAsDensities);

    dataViews.emplace_back(currentDataView);

    addWindow(DummyPtr(*currentDataView), true, .85);
}

void RtoR::Monitor::draw() {
    OpenGLMonitor::draw();
}

void RtoR::Monitor::handleOutput(const OutputPacket &outInfo) {
    const RtoR::EquationState &fieldState = *outInfo.getEqStateData<RtoR::EquationState>();

    hamiltonian.computeEnergies(fieldState);

    for(auto dataView : dataViews)
        dataView->output(outInfo);

    ::Graphics::OpenGLMonitor::handleOutput(outInfo);
}

bool RtoR::Monitor::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {

    if(modKeys.nonePressed() && state == Core::Press)
            switch (key) {
                case '1':
                case '2':
                case '3':
                case '4':
                default:
                    break;
            }

    return OpenGLMonitor::notifyKeyboard(key, state, modKeys);
}

void RtoR::Monitor::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simHistory) {
    simulationHistory = simHistory;

    for(auto dataView : dataViews)
        const_cast<Graphics::RtoRPanel*>(dataView)->setSimulationHistory(simHistory);
}

void RtoR::Monitor::setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory) {
    spaceFTHistory = sftHistory;

    for(auto dataView : dataViews)
        const_cast<Graphics::RtoRPanel*>(dataView)->setSpaceFourierHistory(sftHistory);
}











