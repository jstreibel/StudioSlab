//
// Created by joao on 16/04/24.
//

#ifndef STUDIOSLAB_RTORSTATISTICALMONITOR_H
#define STUDIOSLAB_RTORSTATISTICALMONITOR_H


#include "Graphics/OpenGL/OpenGLMonitor.h"

#include "GraphRtoR.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/CorrelationFunction/CorrelationFunction.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Math/Function/Function.h"

#include "Math/Function/RtoR2/StraightLine.h"

#include "HistoryDisplay.h"
#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "RtoRPanel.h"


namespace RtoR {
    class StatisticalMonitor : public Graphics::RtoRPanel {
        Real Δt = 0.0;
        std::shared_ptr<R2toR::Sampler> sampler;

        std::shared_ptr<RtoR2::StraightLine> correlationLine;
        RtoR::Section1D::Ptr mSpaceCorrelation;
        R2toR::CorrelationFunction mCorrelationFunction;
        std::shared_ptr<R2toR::DiscreteFunction> mCorrelationComputed;
        Graphics::GraphRtoR mCorrelationSectionGraph;
        Graphics::FlatFieldDisplay mCorrelationGraph;


        float t_history = .0f;
        int step_history = 0;

        void updateHistoryGraphs();

        Real transientHint = -1.0;

        KGEnergy &hamiltonian;

        Real u=.0;

        Real barϕ=.0;
        Real tau=.0;
        Real tau_indirect=.0;
        Real tau_avg=.0;

        Spaces::PointSet temperature1HistoryData;
        Spaces::PointSet temperature2HistoryData;
        Spaces::PointSet temperature3HistoryData;
        Spaces::PointSet temperature4HistoryData;

        Graphics::Graph2D mTemperaturesGraph;

        Spaces::PointSet histogramKData;
        Spaces::PointSet histogramGradData;
        Spaces::PointSet histogramVData;
        Spaces::PointSet histogramEData;

        Graphics::Graph2D mHistogramsGraphK;
        Graphics::Graph2D mHistogramsGraphGrad;
        Graphics::Graph2D mHistogramsGraphV;
        Graphics::Graph2D mHistogramsGraphE;

        void drawGUI();

    protected:
        auto handleOutput(const OutputPacket &packet) -> void override;

    public:
        StatisticalMonitor(const NumericConfig &params, KGEnergy &hamiltonian, Graphics::GUIWindow &guiWindow);

        void draw() override;

        void setTransientHint(Real);

        void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                  std::shared_ptr<Graphics::HistoryDisplay> simHistoryGraph) override;

    };
}


#endif //STUDIOSLAB_RTORSTATISTICALMONITOR_H
