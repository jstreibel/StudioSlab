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

    Monitor::Monitor(const TPointer<FKGNumericConfig> &params, FKGEnergy &hamiltonian, const Str &name)
    : BaseMonitor(params->getn(), Str("ℝ↦ℝ ") + name, 10)
    , Hamiltonian(hamiltonian)
    , FullHistoryGraph(   Slab::New<FPlot2DWindow>("Full field history"))
    , FullSFTHistoryGraph(Slab::New<FPlot2DWindow>("Full space FT history"))
    {
        FullHistoryArtist->SetLabel("ϕ(t,x)");
        FullHistoryArtist->SetAffectGraphRanges(true);

        FullHistoryGraph->AddArtist(FullHistoryArtist);
        FullHistoryGraph->GetAxisArtist().setVerticalAxisLabel("t");
        FullHistoryGraph->SetAutoReviewGraphRanges(true);

        FullSFTHistoryArtist->SetLabel("ℱₓ(t,k)");
        FullSFTHistoryArtist->SetAffectGraphRanges(true);

        FullSFTHistoryGraph->AddArtist(FullSFTHistoryArtist);
        FullSFTHistoryGraph->GetAxisArtist().SetHorizontalAxisLabel("k");
        FullSFTHistoryGraph->GetAxisArtist().setVerticalAxisLabel("t");
        FullSFTHistoryGraph->SetAutoReviewGraphRanges(true);


        auto currStyle = Graphics::PlotThemeManager::GetCurrent();

        AddDataView(Slab::New<FRealtimePanel>(params, hamiltonian, *GuiWindow));
        AddDataView(Slab::New<RtoRFourierPanel>(params, hamiltonian, *GuiWindow));
        AddDataView(HistoryPanel = Slab::New<FRtoRHistoryPanel>(params, *GuiWindow, hamiltonian));
        //addDataView(Slab::New<CorrelationsPanel>(params, guiWindow, hamiltonian));
        AddDataView(Slab::New<RtoRStatisticsPanel>(params, hamiltonian, *GuiWindow));
        AddDataView(Slab::New<RtoRScenePanel>(params, *GuiWindow, hamiltonian));

        SetDataView(0);
    }

    void Monitor::AddDataView(const TPointer<FRtoRPanel>
 &dataView) {
        DataViews.emplace_back(dataView);
    }

    void Monitor::SetDataView(int index) {
        if (index > DataViews.size() - 1) return;

        auto oldDataView = CurrentDataView;
        auto newDataView = DataViews[index];

        if (newDataView == oldDataView) return;

        removeWindow(CurrentDataView);

        CurrentDataView = newDataView;

        AddWindow(CurrentDataView, true, -1);

        ArrangeWindows();

        newDataView->notifyBecameVisible();
        if (oldDataView != nullptr) oldDataView->notifyBecameInvisible();
    }

    void Monitor::HandleOutput(const OutputPacket &outInfo) {
        for (const auto &dataView: DataViews)
            dataView->output(outInfo);

        Graphics::BaseMonitor::HandleOutput(outInfo);
    }

    bool Monitor::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {

        auto number = key - '0';

        if (modKeys.nonePressed() && state == EKeyState::Press && (number >= 1 && number <= 9)) {
            SetDataView(number - 1);
            return true;
        }

        return BaseMonitor::NotifyKeyboard(key, state, modKeys);
    }

    void Monitor::SetSimulationHistory(const TPointer<const R2toR::FNumericFunction> &simHistory) {
        SimulationHistory = simHistory;

        FullHistoryArtist->setFunction(SimulationHistory);
        // fullHistoryArtist->setColorMap(Graphics::ColorMaps["BrBG"]->inverse().clone());

        for (const auto &dataView: DataViews)
            dataView->SetSimulationHistory(simHistory, FullHistoryArtist);
    }

    void Monitor::SetSpaceFourierHistory(const TPointer<const R2toR::FNumericFunction> &sftHistory,
                                               const FDFTDataHistory &_dftData) {
        SpaceFTHistory = sftHistory;
        FullSFTHistoryArtist->setFunction(SpaceFTHistory);
        // fullSFTHistoryArtist->setColorMap(Graphics::ColorMaps["blues"]->inverse().bgr().clone());
        FullSFTHistoryGraph->GetAxisArtist().setHorizontalUnit(Constants::π);
        FullSFTHistoryGraph->GetAxisArtist().SetHorizontalAxisLabel("k");
        FullSFTHistoryGraph->GetAxisArtist().setVerticalAxisLabel("t");

        for (const auto &dataView: DataViews)
            dataView->SetSpaceFourierHistory(sftHistory, _dftData, FullSFTHistoryArtist);
    }

    void Monitor::UpdateHistoryGraph() {
        if (SimulationHistory == nullptr) return;

        static bool isSetup = false;
        if (not isSetup && LastPacket.hasValidData()) {

            auto &phi = dynamic_cast<RtoR::NumericFunction&>
                    (LastPacket.GetNakedStateData<EquationState>()->getPhi());

            if (phi.getLaplacianType() == RtoR::NumericFunction::Standard1D_PeriodicBorder)
                FullHistoryArtist->set_xPeriodicOn();

            isSetup = true;
        }

        static DevFloat stepMod, lastStepMod = 0;
        stepMod = (DevFloat) (LastPacket.GetSteps() % (this->getnSteps() * 100));

        fix t = FCommandLineInterfaceManager::getInstance().getParameter("t")->getValueAs<DevFloat>();
        fix steps = FCommandLineInterfaceManager::getInstance().getParameter("m")->getValueAs<Int>();
        fix dt = t/(DevFloat)steps;
        if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
            FullHistoryArtist->set_t((DevFloat)LastPacket.GetSteps()*dt);
        lastStepMod = stepMod;

    }

    void Monitor::UpdateSFTHistoryGraph() {
        if (SpaceFTHistory == nullptr) return;

        fix step = LastPacket.GetSteps();

        static DevFloat stepMod, lastStepMod = 0;
        stepMod = (DevFloat) (step % (this->getnSteps() * 100));
        fix h = FCommandLineInterfaceManager::getInstance().getParameter("h")->getValueAs<DevFloat>();
        fix r = FCommandLineInterfaceManager::getInstance().getParameter("r_dt")->getValueAs<DevFloat>();
        fix dt = h*r;
        if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
            FullSFTHistoryArtist->set_t((DevFloat)LastPacket.GetSteps()*dt);
        lastStepMod = stepMod;
    }

    void Monitor::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        const EquationState &fieldState = *LastPacket.GetNakedStateData<EquationState>();

        auto &phi = dynamic_cast<RtoR::NumericFunction&>(fieldState.getPhi());
        auto &ddtPhi = dynamic_cast<RtoR::NumericFunction&>(fieldState.getDPhiDt());
        Hamiltonian.computeEnergies(phi, ddtPhi);

        UpdateHistoryGraph();
        UpdateSFTHistoryGraph();

        BaseMonitor::ImmediateDraw(PlatformWindow);
    }


}