//
//
// Created by joao on 23/09/2019.


#include "RtoRMonitor.h"

#include <utility>

#include "Models/KleinGordon/KGSolver.h"
#include "Maps/RtoR/Calc/FourierTransform.h"
#include "RtoRRealtimePanel.h"
#include "RtoRStatisticalMonitor.h"


RtoR::Monitor::Monitor(const NumericConfig &params, KGEnergy &hamiltonian,
                       const Real phiMin, const Real phiMax, const Str& name, bool showEnergyHistoryAsDensities)
: ::Graphics::OpenGLMonitor(params, Str("ℝ↦ℝ ") + name)
, hamiltonian(hamiltonian)
{
    auto currStyle = Math::StylesManager::GetCurrent();

    Graphics::RtoRPanel *dataView = new RtoR::RealtimePanel(params, hamiltonian, stats,
                                              phiMin, phiMax, showEnergyHistoryAsDensities);
    dataViews.emplace_back(dataView);

    dataView = new RtoR::StatisticalMonitor(params, hamiltonian, stats);
    dataViews.emplace_back(dataView);

    setDataView(0);
}

void RtoR::Monitor::draw() {
    OpenGLMonitor::draw();
}

void RtoR::Monitor::setDataView(int index) {
    if(index > dataViews.size()-1) return;

    removeWindow(DummyPtr(*currentDataView));
    currentDataView = dataViews[index];
    addWindow(DummyPtr(*currentDataView), true, .85);

    arrangeWindows();
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
                    setDataView(0);
                    break;
                case '2':
                    setDataView(1);
                    break;
                case '3':
                    setDataView(2);
                    break;
                case '4':
                    setDataView(3);
                    break;
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













