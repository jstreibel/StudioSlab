//
//
// Created by joao on 23/09/2019.


#include "RtoRMonitor.h"

#include "Models/KleinGordon/KGSolver.h"
#include "RtoRRealtimePanel.h"
#include "RtoRFourierPanel.h"



// Ok to touch these:
#define UPDATE_HISTORY_EVERY_STEP true



RtoR::Monitor::Monitor(const NumericConfig &params, KGEnergy &hamiltonian,
                       const Real phiMin, const Real phiMax, const Str& name, bool showEnergyHistoryAsDensities)
: ::Graphics::OpenGLMonitor(params, Str("ℝ↦ℝ ") + name)
, hamiltonian(hamiltonian)
{
    auto currStyle = Math::StylesManager::GetCurrent();

    dataViews.emplace_back(new RtoR::RealtimePanel(params, hamiltonian, guiWindow, phiMin, phiMax, showEnergyHistoryAsDensities));
    dataViews.emplace_back(new Graphics::RtoRFourierPanel(params, hamiltonian, guiWindow));

    setDataView(0);
}

void RtoR::Monitor::addDataView(std::shared_ptr<Graphics::RtoRPanel> dataView){
    dataViews.emplace_back(dataView);
}

void RtoR::Monitor::setDataView(int index) {
    if(index > dataViews.size()-1) return;

    auto oldDataView = currentDataView;
    auto newDataView= dataViews[index];

    if(newDataView==oldDataView) return;

    removeWindow(currentDataView);

    currentDataView = newDataView;

    addWindow(currentDataView, true, -1);

    arrangeWindows();

    newDataView->notifyBecameVisible();
    if(oldDataView != nullptr) oldDataView->notifyBecameInvisible();
}

void RtoR::Monitor::handleOutput(const OutputPacket &outInfo) {
    const RtoR::EquationState &fieldState = *outInfo.getEqStateData<RtoR::EquationState>();

    hamiltonian.computeEnergies(fieldState);

    updateHistoryGraph();
    updateSFTHistoryGraph();

    for(auto dataView : dataViews)
        dataView->output(outInfo);

    ::Graphics::OpenGLMonitor::handleOutput(outInfo);
}

bool RtoR::Monitor::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {

    char number = key-'0';

    if(modKeys.nonePressed() && state == Core::Press && (number>=1 && number<=9)) {
        setDataView(number-1);
        return true;
    }

    return OpenGLMonitor::notifyKeyboard(key, state, modKeys);
}

void RtoR::Monitor::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simHistory) {
    simulationHistory = simHistory;
    fullHistoryGraph = Pointer(new Graphics::HistoryDisplay("Full field history"));
    fullHistoryGraph->addFunction(simulationHistory, "ϕ(t,x)", -100);

    fullHistoryGraph->setColorMap(Styles::ColorMaps["BrBG"].inverse());

    for(auto dataView : dataViews)
        dataView->setSimulationHistory(simHistory, fullHistoryGraph);
}

void RtoR::Monitor::setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                           const SimHistory_DFT::DFTDataHistory &_dftData)
{
    this->dftData = &_dftData;

    spaceFTHistory = sftHistory;
    fullSFTHistoryGraph = Pointer(new Graphics::HistoryDisplay("Space DFT history"));
    fullSFTHistoryGraph->addFunction(spaceFTHistory, "ℱ[ϕ(t)](k)");

    fullSFTHistoryGraph->setColorMap(Styles::ColorMaps["blues"].inverse().bgr());
    fullSFTHistoryGraph->getAxisArtist().setHorizontalUnit(Constants::π);
    fullSFTHistoryGraph->getAxisArtist().setHorizontalAxisLabel("k");
    fullSFTHistoryGraph->getAxisArtist().setVerticalAxisLabel("t");

    for(auto dataView : dataViews)
        dataView->setSpaceFourierHistory(sftHistory, _dftData, fullSFTHistoryGraph);
}

void RtoR::Monitor::updateHistoryGraph() {
    if(simulationHistory == nullptr) return;

    static bool isSetup = false;
    if( not isSetup && lastData.hasValidData()) {

        auto &phi = lastData.getEqStateData<RtoR::EquationState>()->getPhi();
        if(phi.getLaplacianType() == DiscreteFunction::Standard1D_PeriodicBorder)
            fullHistoryGraph->set_xPeriodicOn();

        isSetup = true;
    }

    {
        static Real stepMod, lastStepMod = 0;
        stepMod = (Real) (lastData.getSteps() % (this->getnSteps() * 100));
        if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
            fullHistoryGraph->set_t(lastData.getSimTime());
        lastStepMod = stepMod;
    }
}

void RtoR::Monitor::updateSFTHistoryGraph() {
    if(spaceFTHistory == nullptr) return;

    fix step = lastData.getSteps();

    static Real stepMod, lastStepMod = 0;
    stepMod = (Real) (step % (this->getnSteps() * 100));
    if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
        fullSFTHistoryGraph->set_t(lastData.getSimTime());
    lastStepMod = stepMod;
}
