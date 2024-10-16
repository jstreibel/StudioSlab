//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTORMONITOR_BASE_H
#define V_SHAPE_RTORMONITOR_BASE_H

#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/BaseMonitor.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/HistoryArtist.h"

#include "RtoRPanel.h"
#include "Utils/Threads.h"
#include "Models/KleinGordon/KGNumericConfig.h"


namespace Slab::Graphics {
    class HistoryDisplay;
}

namespace Slab::Math::R2toR {
    class NumericFunction;
}

namespace Slab::Models::KGRtoR {
    class KGEnergy;

    class Monitor : public Graphics::BaseMonitor {
        Vector<RtoRPanel_ptr> dataViews;
        RtoRPanel_ptr currentDataView;

        R2toR::NumericFunction_constptr simulationHistory;
        Pointer<Graphics::Plot2DWindow> fullHistoryGraph = Slab::New<Graphics::Plot2DWindow>("Full field history");
        Graphics::HistoryArtist_ptr fullHistoryArtist = Slab::New<Graphics::HistoryArtist>();

        R2toR::NumericFunction_constptr spaceFTHistory;
        Pointer<Graphics::Plot2DWindow> fullSFTHistoryGraph = Slab::New<Graphics::Plot2DWindow>("Full space FT history");
        Graphics::HistoryArtist_ptr fullSFTHistoryArtist = Slab::New<Graphics::HistoryArtist>();

        KGEnergy &hamiltonian;

        void setDataView(int index);

        void updateHistoryGraph();

        void updateSFTHistoryGraph();

    protected:
        void handleOutput(const OutputPacket &outInfo) override;

    public:
        Monitor(const Pointer<KGNumericConfig> &params,
                KGEnergy &hamiltonian,
                Real phiMin=-1,
                Real phiMax=1,
                const Str& name = "general graphic monitor",
                bool showEnergyHistoryAsDensities=false);

        void draw() override;

        virtual void setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory);
        virtual void setSpaceFourierHistory(R2toR::NumericFunction_constptr sftHistory,
                                            const DFTDataHistory &dftData);

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        void addDataView(const RtoRPanel_ptr&);
    };

}


#endif // V_SHAPE_RTORMONITOR_BASE_H

