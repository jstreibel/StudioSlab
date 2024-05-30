//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTORMONITOR_BASE_H
#define V_SHAPE_RTORMONITOR_BASE_H

#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/BaseMonitor.h"
#include "Graphics/Graph/PlottingWindow.h"
#include "Graphics/Graph/Artists/HistoryArtist.h"

#include "RtoRPanel.h"
#include "Utils/Threads.h"


namespace Slab::Graphics {
    class HistoryDisplay;
}

namespace Slab::Math::R2toR {
    class DiscreteFunction;
}

namespace Slab::Models::KGRtoR {
    class KGEnergy;

    class Monitor : public Graphics::BaseMonitor {
        Vector<RtoRPanel_ptr> dataViews;
        RtoRPanel_ptr currentDataView;

        R2toR::DiscreteFunction_constptr simulationHistory;
        Graphics::PlottingWindow_ptr fullHistoryGraph = Slab::New<Graphics::PlottingWindow>("Full field history");
        Graphics::HistoryArtist_ptr fullHistoryArtist = Slab::New<Graphics::HistoryArtist>();

        R2toR::DiscreteFunction_constptr spaceFTHistory;
        Graphics::PlottingWindow_ptr fullSFTHistoryGraph = Slab::New<Graphics::PlottingWindow>("Full space FT history");
        Graphics::HistoryArtist_ptr fullSFTHistoryArtist = Slab::New<Graphics::HistoryArtist>();

        KGEnergy &hamiltonian;

        void setDataView(int index);

        void updateHistoryGraph();

        void updateSFTHistoryGraph();

    protected:
        void handleOutput(const OutputPacket &outInfo) override;

    public:
        Monitor(const NumericConfig &params,
                KGEnergy &hamiltonian,
                Real phiMin=-1,
                Real phiMax=1,
                const Str& name = "general graphic monitor",
                bool showEnergyHistoryAsDensities=false);

        void draw() override;

        virtual void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory);
        virtual void setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                            const DFTDataHistory &dftData);

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        void addDataView(const RtoRPanel_ptr&);
    };

}


#endif // V_SHAPE_RTORMONITOR_BASE_H
