//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGL_H
#define V_SHAPE_RTOROUTPUTOPENGL_H

#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "Mappings/RtoR/Model/RtoRResizableDiscreteFunction.h"
#include "Base/Graphics/Window/WindowContainer/WindowPanel.h"

#include "Mappings/RtoR/View/Graphics/GraphRtoR.h"
#include "Phys/Graph/Monitor.h"
#include "Phys/Graph/PointSetGraph.h"

namespace RtoR {

    class Monitor : public Graphics::Monitor {
        bool notifyKeyboard(unsigned char key, int x, int y) override;

    protected:
        KGEnergy &hamiltonian;
        GraphRtoR mFieldsGraph;

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

        void draw() override;
        void _out(const OutputPacket &outInfo) override;

    public:
        Monitor(const NumericParams &params, KGEnergy &hamiltonian, Real phiMin=-1, Real phiMax=1);

    };

}


#endif //V_SHAPE_RTOROUTPUTOPENGL_H
