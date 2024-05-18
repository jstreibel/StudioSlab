//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORPANEL_H
#define STUDIOSLAB_RTORPANEL_H

#include "Math/Numerics/Output/Plugs/Socket.h"

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"

#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"
#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"

#include "HistoryDisplay.h"
#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"

namespace Graphics {

    class RtoRPanel : public Graphics::WindowPanel, public Numerics::OutputSystem::Socket {
    protected:
        Graphics::GUIWindow &guiWindow;
        const NumericConfig &params;
        RtoR::KGEnergy &hamiltonian;

        std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory = nullptr;
        std::shared_ptr<HistoryDisplay> simulationHistoryGraph = nullptr;

        std::shared_ptr<const R2toR::DiscreteFunction> spaceFTHistory = nullptr;
        std::shared_ptr<HistoryDisplay> spaceFTHistoryGraph = nullptr;
        const DFTDataHistory *dftData;

        auto handleOutput(const OutputPacket &packet) -> void override;

    public:
        RtoRPanel(const NumericConfig &params,
                  GUIWindow &guiWindow,
                  RtoR::KGEnergy &hamiltonian,
                  const Str &name,
                  const Str &description);

        virtual void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                          std::shared_ptr<HistoryDisplay> simHistoryGraph);
        virtual void setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                            const DFTDataHistory &,
                                            std::shared_ptr<HistoryDisplay> sftHistoryGraph);

        virtual void notifyBecameVisible();
        virtual void notifyBecameInvisible();

    };

} // Graphics

#endif //STUDIOSLAB_RTORPANEL_H
