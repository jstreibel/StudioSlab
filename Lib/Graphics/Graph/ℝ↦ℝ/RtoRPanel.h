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

#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"
#include "Graphics/Graph/PlottingWindow.h"

namespace Graphics {

    class RtoRPanel : public Graphics::WindowPanel, public Numerics::OutputSystem::Socket {
    protected:
        Graphics::GUIWindow &guiWindow;
        const NumericConfig &params;
        RtoR::KGEnergy &hamiltonian;

        R2toR::DiscreteFunction_constptr simulationHistory;
        PlottingWindow_ptr simulationHistoryGraph;

        R2toR::DiscreteFunction_constptr spaceFTHistory;
        PlottingWindow_ptr spaceFTHistoryGraph;
        const DFTDataHistory *dftData;

        auto handleOutput(const OutputPacket &packet) -> void override;

    public:
        RtoRPanel(const NumericConfig &params,
                  GUIWindow &guiWindow,
                  RtoR::KGEnergy &hamiltonian,
                  const Str &name,
                  const Str &description);

        virtual void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                          PlottingWindow_ptr simHistoryGraph);
        virtual void setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                            const DFTDataHistory &,
                                            PlottingWindow_ptr sftHistoryGraph);

        virtual void notifyBecameVisible();
        virtual void notifyBecameInvisible();

    };

    DefinePointer(RtoRPanel)

} // Graphics

#endif //STUDIOSLAB_RTORPANEL_H
