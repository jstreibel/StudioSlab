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
        R2toR::Sampler_ptr sampler;

        RtoR2::StraightLine_ptr correlationLine;
        RtoR::Section1D_ptr mSpaceCorrelation;
        R2toR::CorrelationFunction mCorrelationFunction;
        R2toR::DiscreteFunction_ptr mCorrelationComputed;
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

        Math::PointSet temperature1HistoryData;
        Math::PointSet temperature2HistoryData;
        Math::PointSet temperature3HistoryData;
        Math::PointSet temperature4HistoryData;

        Graphics::Graph2D mTemperaturesGraph;

        Math::PointSet histogramKData;
        Math::PointSet histogramGradData;
        Math::PointSet histogramVData;
        Math::PointSet histogramEData;

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

        void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                  Graphics::HistoryDisplay_ptr simHistoryGraph) override;

    };
}


#endif //STUDIOSLAB_RTORSTATISTICALMONITOR_H
