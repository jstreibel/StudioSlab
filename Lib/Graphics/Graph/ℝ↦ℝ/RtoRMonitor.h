//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGL_H
#define V_SHAPE_RTOROUTPUTOPENGL_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Graphics/OpenGL/OpenGLMonitor.h"

#include "Maps/RtoR/Model/RtoRResizableDiscreteFunction.h"
#include "Maps/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Maps/R2toR/Model/FunctionsCollection/CorrelationFunction/Sampler.h"
#include "Maps/R2toR/Model/FunctionsCollection/CorrelationFunction/CorrelationFunction.h"

#include "HistoryDisplay.h"
#include "Maps/RtoR2/StraightLine.h"
#include "Graphics/Graph/ℝ↦ℝ/GraphRtoR.h"
#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"
#include "RtoRPanel.h"

namespace RtoR {

    class Monitor : public ::Graphics::OpenGLMonitor {
        std::vector<Graphics::RtoRPanel*> dataViews;
        Graphics::RtoRPanel* currentDataView = nullptr;

    public:
        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

    protected:
        std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory = nullptr;
        std::shared_ptr<const R2toR::DiscreteFunction> spaceFTHistory = nullptr;

        KGEnergy &hamiltonian;

        void draw() override;

        void handleOutput(const OutputPacket &outInfo) override;

    public:
        Monitor(const NumericConfig &params,
                KGEnergy &hamiltonian,
                Real phiMin=-1,
                Real phiMax=1,
                const Str& name = "general graphic monitor",
                bool showEnergyHistoryAsDensities=false);

        virtual void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory);
        virtual void setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory);
    };

}


#endif //V_SHAPE_RTOROUTPUTOPENGL_H

