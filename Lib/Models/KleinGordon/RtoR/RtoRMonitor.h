//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGL_H
#define V_SHAPE_RTOROUTPUTOPENGL_H

#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "Mappings/RtoR/Model/RtoRResizableDiscreteFunction.h"
#include "Base/Graphics/Window/WindowContainer/WindowPanel.h"

#include "Mappings/RtoR/View/Graphics/GraphRtoR.h"
#include "Phys/Graph/OpenGLMonitor.h"
#include "Phys/Graph/PointSetGraph.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"

namespace RtoR {

    class Monitor : public Graphics::OpenGLMonitor {
        bool showEnergyHistoryAsDensities;

        bool notifyKeyboard(unsigned char key, int x, int y) override;

    protected:

        Real U;
        Real K;
        Real V;
        Real W;
        KGEnergy &hamiltonian;
        GraphRtoR mFieldsGraph;
        GraphRtoR mHistoryGraph;

        std::shared_ptr<const R2toR::DiscreteFunction>
                  simulationHistory = nullptr;

        Spaces::PointSet UHistoryData;
        Spaces::PointSet KHistoryData;
        Spaces::PointSet WHistoryData;
        Spaces::PointSet VHistoryData;
        Fields::Graphing::PointSetGraph mEnergyGraph;

        const Styles::PlotStyle U_style = Styles::GetColorScheme()->funcPlotStyles[0];
        const Styles::PlotStyle K_style = Styles::GetColorScheme()->funcPlotStyles[1];
        const Styles::PlotStyle W_style = Styles::GetColorScheme()->funcPlotStyles[2];
        const Styles::PlotStyle V_style = Styles::GetColorScheme()->funcPlotStyles[3];

        bool showPhi = true;
        bool showKineticEnergy = false;
        bool showGradientEnergy = false;
        bool showEnergyDensity = false;

        float t_history = .0f;
        int step_history = 0;

        virtual void draw() override;

        virtual void handleOutput(const OutputPacket &outInfo) override;

    public:
        Monitor(const NumericConfig &params,
                KGEnergy &hamiltonian,
                Real phiMin=-1,
                Real phiMax=1,
                Str name = "general graphic monitor",
                bool showEnergyHistoryAsDensities=false);

        virtual void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory);
    };

}


#endif //V_SHAPE_RTOROUTPUTOPENGL_H
