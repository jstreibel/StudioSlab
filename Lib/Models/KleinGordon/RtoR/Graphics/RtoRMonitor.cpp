//
//
// Created by joao on 23/09/2019.


#include "RtoRMonitor.h"

#include "Models/KleinGordon/KGSolver.h"
#include "RtoRRealtimePanel.h"
#include "RtoRFourierPanel.h"
#include "CorrelationsPanel.h"


// Ok to touch these:
#define UPDATE_HISTORY_EVERY_STEP true


namespace Slab::Models::KGRtoR {

    Monitor::Monitor(const NumericConfig &params, KGEnergy &hamiltonian,
                           const Real phiMin, const Real phiMax, const Str &name, bool showEnergyHistoryAsDensities)
            : Graphics::OpenGLMonitor(params, Str("ℝ↦ℝ ") + name), hamiltonian(hamiltonian) {
        fullHistoryArtist->setLabel("ϕ(t,x)");
        fullHistoryGraph->addArtist(fullHistoryArtist);

        fullSFTHistoryArtist->setLabel("ℱₓ(t,k)");
        fullSFTHistoryGraph->addArtist(fullSFTHistoryArtist);


        auto currStyle = Graphics::PlotThemeManager::GetCurrent();

        addDataView(Slab::New<RealtimePanel>(params, hamiltonian, guiWindow));
        addDataView(Slab::New<RtoRFourierPanel>(params, hamiltonian, guiWindow));
        addDataView(Slab::New<CorrelationsPanel>(params, guiWindow, hamiltonian));

        setDataView(0);
    }

    void Monitor::addDataView(const RtoRPanel_ptr &dataView) {
        dataViews.emplace_back(dataView);
    }

    void Monitor::setDataView(int index) {
        if (index > dataViews.size() - 1) return;

        auto oldDataView = currentDataView;
        auto newDataView = dataViews[index];

        if (newDataView == oldDataView) return;

        removeWindow(currentDataView);

        currentDataView = newDataView;

        addWindow(currentDataView, true, -1);

        arrangeWindows();

        newDataView->notifyBecameVisible();
        if (oldDataView != nullptr) oldDataView->notifyBecameInvisible();
    }

    void Monitor::handleOutput(const OutputPacket &outInfo) {
        const EquationState &fieldState = *outInfo.getEqStateData<EquationState>();

        hamiltonian.computeEnergies(fieldState);

        // updateHistoryGraph();
        // updateSFTHistoryGraph();

        for (const auto &dataView: dataViews)
            dataView->output(outInfo);

        Graphics::OpenGLMonitor::handleOutput(outInfo);
    }

    bool Monitor::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {

        char number = key - '0';

        if (modKeys.nonePressed() && state == Core::Press && (number >= 1 && number <= 9)) {
            setDataView(number - 1);
            return true;
        }

        return OpenGLMonitor::notifyKeyboard(key, state, modKeys);
    }

    void Monitor::setSimulationHistory(R2toR::DiscreteFunction_constptr simHistory) {
        simulationHistory = simHistory;

        fullHistoryArtist->setFunction(simulationHistory);
        fullHistoryArtist->setColorMap(Graphics::ColorMaps["BrBG"].inverse());

        for (const auto &dataView: dataViews)
            dataView->setSimulationHistory(simHistory, fullHistoryGraph);
    }

    void Monitor::setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                               const DFTDataHistory &_dftData) {
        this->dftData = &_dftData;

        spaceFTHistory = sftHistory;
        fullSFTHistoryArtist->setFunction(spaceFTHistory);
        fullSFTHistoryArtist->setColorMap(Graphics::ColorMaps["blues"].inverse().bgr());
        fullSFTHistoryGraph->getAxisArtist().setHorizontalUnit(Constants::π);
        fullSFTHistoryGraph->getAxisArtist().setHorizontalAxisLabel("k");
        fullSFTHistoryGraph->getAxisArtist().setVerticalAxisLabel("t");

        for (const auto &dataView: dataViews)
            dataView->setSpaceFourierHistory(sftHistory, _dftData, fullSFTHistoryGraph);
    }

    void Monitor::updateHistoryGraph() {
        if (simulationHistory == nullptr) return;

        static bool isSetup = false;
        if (not isSetup && lastData.hasValidData()) {

            auto &phi = lastData.getEqStateData<EquationState>()->getPhi();
            if (phi.getLaplacianType() == RtoR::DiscreteFunction::Standard1D_PeriodicBorder)
                fullHistoryArtist->set_xPeriodicOn();

            isSetup = true;
        }

        {
            static Real stepMod, lastStepMod = 0;
            stepMod = (Real) (lastData.getSteps() % (this->getnSteps() * 100));
            if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
                fullHistoryArtist->set_t(lastData.getSimTime());
            lastStepMod = stepMod;
        }
    }

    void Monitor::updateSFTHistoryGraph() {
        if (spaceFTHistory == nullptr) return;

        fix step = lastData.getSteps();

        static Real stepMod, lastStepMod = 0;
        stepMod = (Real) (step % (this->getnSteps() * 100));
        if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
            fullSFTHistoryArtist->set_t(lastData.getSimTime());
        lastStepMod = stepMod;
    }

    void Monitor::draw() {
        updateHistoryGraph();
        updateSFTHistoryGraph();

        WindowPanel::draw();
    }


}