
//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTORMONITOR_BASE_H
#define V_SHAPE_RTORMONITOR_BASE_H

#include "Models/KleinGordon/RtoR/LinearStepping/Output/SimHistory_Fourier.h"

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/BaseMonitor.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/HistoryArtist.h"

#include "RtoRPanel.h"
#include "Utils/Threads.h"
#include "Models/KleinGordon/KG-NumericConfig.h"
#include "Panels/RtoRHistoryPanel.h"


namespace Slab::Graphics {
    class HistoryDisplay;
}

namespace Slab::Math::R2toR {
    class FNumericFunction;
}

namespace Slab::Models::KGRtoR {
    class FKGEnergy;

    class Monitor : public Graphics::BaseMonitor {
    protected:
        Vector<FRtoRPanel_ptr> DataViews;
        TPointer<FRtoRPanel> CurrentDataView;

        TPointer<const R2toR::FNumericFunction> SimulationHistory;
        TPointer<FPlot2DWindow> FullHistoryGraph;
        HistoryArtist_ptr FullHistoryArtist = Slab::New<HistoryArtist>();

        TPointer<const R2toR::FNumericFunction> SpaceFTHistory;
        TPointer<FPlot2DWindow> FullSFTHistoryGraph;
        HistoryArtist_ptr FullSFTHistoryArtist = Slab::New<HistoryArtist>();

        TPointer<FRtoRHistoryPanel> HistoryPanel;

        FKGEnergy &Hamiltonian;

        void SetDataView(int index);

        void UpdateHistoryGraph();

        void UpdateSFTHistoryGraph();

    protected:
        void HandleOutput(const FOutputPacket &outInfo) override;

    public:
        Monitor(const TPointer<FKGNumericConfig> &params,
                FKGEnergy &hamiltonian,
                const Str& name = "general graphic monitor");

        void ImmediateDraw(const FPlatformWindow&) override;

        virtual void SetSimulationHistory  (const TPointer<const R2toR::FNumericFunction> &simulationHistory);
        virtual void SetSpaceFourierHistory(const TPointer<const R2toR::FNumericFunction> &sftHistory,
                                            const FDFTDataHistory &dftData);

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        void AddDataView(const TPointer<FRtoRPanel>
&);
    };

}


#endif // V_SHAPE_RTORMONITOR_BASE_H

