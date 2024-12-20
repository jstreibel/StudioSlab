
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
                const Str& name = "general graphic monitor");

        void draw() override;

        virtual void setSimulationHistory  (const Pointer<const R2toR::NumericFunction> &simulationHistory);
        virtual void setSpaceFourierHistory(const Pointer<const R2toR::NumericFunction> &sftHistory,
                                            const DFTDataHistory &dftData);

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        void addDataView(const RtoRPanel_ptr&);
    };

}


#endif // V_SHAPE_RTORMONITOR_BASE_H

