//
//
// Created by joao on 23/09/2019.


#include "RtoRMonitor.h"

#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"

#include "Models/KleinGordon/KG-Solver.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRRealtimePanel.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRFourierPanel.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRHistoryPanel.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRScenePanel.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRStatisticalMonitor.h"


// Ok to touch these:
#define UPDATE_HISTORY_EVERY_STEP true


namespace Slab::Models::KGRtoR {

    Monitor::Monitor(const Pointer<KGNumericConfig> &params, KGEnergy &hamiltonian, const Str &name)
            : Graphics::BaseMonitor(params->getn(), Str("ℝ↦ℝ ") + name, 10)
            , hamiltonian(hamiltonian) {
        fullHistoryArtist->setLabel("ϕ(t,x)");
        fullHistoryArtist->setAffectGraphRanges(true);

        fullHistoryGraph->addArtist(fullHistoryArtist);
        fullHistoryGraph->GetAxisArtist().setVerticalAxisLabel("t");
        fullHistoryGraph->SetAutoReviewGraphRanges(true);

        fullSFTHistoryArtist->setLabel("ℱₓ(t,k)");
        fullSFTHistoryArtist->setAffectGraphRanges(true);

        fullSFTHistoryGraph->addArtist(fullSFTHistoryArtist);
        fullSFTHistoryGraph->GetAxisArtist().SetHorizontalAxisLabel("k");
        fullSFTHistoryGraph->GetAxisArtist().setVerticalAxisLabel("t");
        fullSFTHistoryGraph->SetAutoReviewGraphRanges(true);


        auto currStyle = Graphics::PlotThemeManager::GetCurrent();

        addDataView(Slab::New<FRealtimePanel>(params, hamiltonian, *guiWindow));
        addDataView(Slab::New<RtoRFourierPanel>(params, hamiltonian, *guiWindow));
        addDataView(historyPanel = Slab::New<RtoRHistoryPanel>(params, *guiWindow, hamiltonian));
        //addDataView(Slab::New<CorrelationsPanel>(params, guiWindow, hamiltonian));
        addDataView(Slab::New<RtoRStatisticsPanel>(params, hamiltonian, *guiWindow));
        addDataView(Slab::New<RtoRScenePanel>(params, *guiWindow, hamiltonian));

        setDataView(0);
    }

    void Monitor::addDataView(const Pointer<FRtoRPanel>
 &dataView) {
        dataViews.emplace_back(dataView);
    }

    void Monitor::setDataView(int index) {
        if (index > dataViews.size() - 1) return;

        auto oldDataView = currentDataView;
        auto newDataView = dataViews[index];

        if (newDataView == oldDataView) return;

        removeWindow(currentDataView);

        currentDataView = newDataView;

        AddWindow(currentDataView, true, -1);

        arrangeWindows();

        newDataView->notifyBecameVisible();
        if (oldDataView != nullptr) oldDataView->notifyBecameInvisible();
    }

    void Monitor::handleOutput(const OutputPacket &outInfo) {
        for (const auto &dataView: dataViews)
            dataView->output(outInfo);

        Graphics::BaseMonitor::handleOutput(outInfo);
    }

    bool Monitor::NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {

        auto number = key - '0';

        if (modKeys.nonePressed() && state == KeyState::Press && (number >= 1 && number <= 9)) {
            setDataView(number - 1);
            return true;
        }

        return BaseMonitor::NotifyKeyboard(key, state, modKeys);
    }

    void Monitor::setSimulationHistory(const Pointer<const R2toR::FNumericFunction> &simHistory) {
        simulationHistory = simHistory;

        fullHistoryArtist->setFunction(simulationHistory);
        // fullHistoryArtist->setColorMap(Graphics::ColorMaps["BrBG"]->inverse().clone());

        for (const auto &dataView: dataViews)
            dataView->SetSimulationHistory(simHistory, fullHistoryArtist);
    }

    void Monitor::SetSpaceFourierHistory(const Pointer<const R2toR::FNumericFunction> &sftHistory,
                                               const FDFTDataHistory &_dftData) {
        spaceFTHistory = sftHistory;
        fullSFTHistoryArtist->setFunction(spaceFTHistory);
        // fullSFTHistoryArtist->setColorMap(Graphics::ColorMaps["blues"]->inverse().bgr().clone());
        fullSFTHistoryGraph->GetAxisArtist().setHorizontalUnit(Constants::π);
        fullSFTHistoryGraph->GetAxisArtist().SetHorizontalAxisLabel("k");
        fullSFTHistoryGraph->GetAxisArtist().setVerticalAxisLabel("t");

        for (const auto &dataView: dataViews)
            dataView->SetSpaceFourierHistory(sftHistory, _dftData, fullSFTHistoryArtist);
    }

    void Monitor::updateHistoryGraph() {
        if (simulationHistory == nullptr) return;

        static bool isSetup = false;
        if (not isSetup && LastPacket.hasValidData()) {

            auto &phi = dynamic_cast<RtoR::NumericFunction&>
                    (LastPacket.GetNakedStateData<EquationState>()->getPhi());

            if (phi.getLaplacianType() == RtoR::NumericFunction::Standard1D_PeriodicBorder)
                fullHistoryArtist->set_xPeriodicOn();

            isSetup = true;
        }

        static DevFloat stepMod, lastStepMod = 0;
        stepMod = (DevFloat) (LastPacket.GetSteps() % (this->getnSteps() * 100));

        fix t = FCommandLineInterfaceManager::getInstance().getParameter("t")->getValueAs<DevFloat>();
        fix steps = FCommandLineInterfaceManager::getInstance().getParameter("m")->getValueAs<Int>();
        fix dt = t/(DevFloat)steps;
        if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
            fullHistoryArtist->set_t((DevFloat)LastPacket.GetSteps()*dt);
        lastStepMod = stepMod;

    }

    void Monitor::updateSFTHistoryGraph() {
        if (spaceFTHistory == nullptr) return;

        fix step = LastPacket.GetSteps();

        static DevFloat stepMod, lastStepMod = 0;
        stepMod = (DevFloat) (step % (this->getnSteps() * 100));
        fix h = FCommandLineInterfaceManager::getInstance().getParameter("h")->getValueAs<DevFloat>();
        fix r = FCommandLineInterfaceManager::getInstance().getParameter("r_dt")->getValueAs<DevFloat>();
        fix dt = h*r;
        if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
            fullSFTHistoryArtist->set_t((DevFloat)LastPacket.GetSteps()*dt);
        lastStepMod = stepMod;
    }

    void Monitor::Draw() {
        const EquationState &fieldState = *LastPacket.GetNakedStateData<EquationState>();

        auto &phi = dynamic_cast<RtoR::NumericFunction&>(fieldState.getPhi());
        auto &ddtPhi = dynamic_cast<RtoR::NumericFunction&>(fieldState.getDPhiDt());
        hamiltonian.computeEnergies(phi, ddtPhi);

        updateHistoryGraph();
        updateSFTHistoryGraph();

        Slab::Graphics::BaseMonitor::Draw();
    }


}