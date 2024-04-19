//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORPANEL_H
#define STUDIOSLAB_RTORPANEL_H

#include "Math/Numerics/Output/Plugs/Socket.h"

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"

#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"

namespace Graphics {

    class RtoRPanel : public Graphics::WindowPanel, public Numerics::OutputSystem::Socket {
    protected:
        Graphics::GUIWindow &guiWindow;
        const NumericConfig &params;
        RtoR::KGEnergy &hamiltonian;

        std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory = nullptr;
        std::shared_ptr<const R2toR::DiscreteFunction> spaceFTHistory = nullptr;

    public:
        RtoRPanel(const NumericConfig &params,
                  GUIWindow &guiWindow,
                  RtoR::KGEnergy &hamiltonian,
                  const Str &name,
                  const Str &description);

        virtual void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory);
        virtual void setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory);
    };

} // Graphics

#endif //STUDIOSLAB_RTORPANEL_H
