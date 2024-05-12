//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTORMONITOR_BASE_H
#define V_SHAPE_RTORMONITOR_BASE_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Graphics/OpenGL/OpenGLMonitor.h"

#include "Math/Function/Maps/RtoR/Model/RtoRResizableDiscreteFunction.h"
#include "Math/Function/Maps/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Math/Function/Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Math/Function/Maps/R2toR/Model/FunctionsCollection/CorrelationFunction/Sampler.h"
#include "Math/Function/Maps/R2toR/Model/FunctionsCollection/CorrelationFunction/CorrelationFunction.h"

#include "HistoryDisplay.h"
#include "Math/Function/Maps/RtoR2/StraightLine.h"
#include "Graphics/Graph/ℝ↦ℝ/GraphRtoR.h"
#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"
#include "RtoRPanel.h"
#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"

namespace RtoR {

    class Monitor : public ::Graphics::OpenGLMonitor {
        std::vector<std::shared_ptr<Graphics::RtoRPanel>> dataViews;
        std::shared_ptr<Graphics::RtoRPanel> currentDataView = nullptr;

        std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory = nullptr;
        std::shared_ptr<Graphics::HistoryDisplay> fullHistoryGraph = nullptr;
        std::shared_ptr<const R2toR::DiscreteFunction> spaceFTHistory = nullptr;
        std::shared_ptr<Graphics::HistoryDisplay> fullSFTHistoryGraph = nullptr;
        const SimHistory_DFT::DFTDataHistory *dftData;

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

        virtual void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory);
        virtual void setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                            const SimHistory_DFT::DFTDataHistory &dftData);

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        void addDataView(std::shared_ptr<Graphics::RtoRPanel>);
    };

}


#endif // V_SHAPE_RTORMONITOR_BASE_H

