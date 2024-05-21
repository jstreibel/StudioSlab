//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTORMONITOR_BASE_H
#define V_SHAPE_RTORMONITOR_BASE_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Graphics/OpenGL/OpenGLMonitor.h"

#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"
#include "RtoRPanel.h"

namespace Graphics {
    class HistoryDisplay;
}

namespace R2toR {
    class DiscreteFunction;
}

namespace RtoR {
    class KGEnergy;

    class Monitor : public ::Graphics::OpenGLMonitor {
        std::vector<Graphics::RtoRPanel_ptr> dataViews;
        Graphics::RtoRPanel_ptr currentDataView;

        R2toR::DiscreteFunction_constptr simulationHistory;
        Graphics::HistoryDisplay_ptr fullHistoryGraph;
        R2toR::DiscreteFunction_constptr spaceFTHistory;
        Graphics::HistoryDisplay_ptr fullSFTHistoryGraph;
        const DFTDataHistory *dftData;

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

        virtual void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory);
        virtual void setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                            const DFTDataHistory &dftData);

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        void addDataView(const Graphics::RtoRPanel_ptr&);
    };

}


#endif // V_SHAPE_RTORMONITOR_BASE_H

