
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
    class KGEnergy;

    class Monitor : public Graphics::BaseMonitor {
    protected:
        Vector<FRtoRPanel_ptr> dataViews;
        TPointer<FRtoRPanel> currentDataView;

        TPointer<const R2toR::FNumericFunction> simulationHistory;
        TPointer<FPlot2DWindow> fullHistoryGraph;
        HistoryArtist_ptr fullHistoryArtist = Slab::New<HistoryArtist>();

        TPointer<const R2toR::FNumericFunction> spaceFTHistory;
        TPointer<FPlot2DWindow> fullSFTHistoryGraph;
        HistoryArtist_ptr fullSFTHistoryArtist = Slab::New<HistoryArtist>();

        TPointer<RtoRHistoryPanel> historyPanel;

        KGEnergy &hamiltonian;

        void setDataView(int index);

        void updateHistoryGraph();

        void updateSFTHistoryGraph();

    protected:
        void handleOutput(const OutputPacket &outInfo) override;

    public:
        Monitor(const TPointer<FKGNumericConfig> &params,
                KGEnergy &hamiltonian,
                const Str& name = "general graphic monitor");

        void ImmediateDraw(const FPlatformWindow&) override;

        virtual void setSimulationHistory  (const TPointer<const R2toR::FNumericFunction> &simulationHistory);
        virtual void SetSpaceFourierHistory(const TPointer<const R2toR::FNumericFunction> &sftHistory,
                                            const FDFTDataHistory &dftData);

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        void addDataView(const TPointer<FRtoRPanel>
&);
    };

}


#endif // V_SHAPE_RTORMONITOR_BASE_H

