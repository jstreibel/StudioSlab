//
// Created by joao on 7/12/22.
//

#ifndef STUDIOSLAB_THERMALMONITOR_H
#define STUDIOSLAB_THERMALMONITOR_H

#include "Models/KleinGordon/RtoR/RtoRMonitor.h"
#include "Mappings/R2toR/View/Artists/FlatFieldDisplay.h"
#include "Mappings/RtoR/View/Graphics/HistoryDisplay.h"
#include "Mappings/R2toR/Model/FunctionsCollection/CorrelationFunction/CorrelationFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/Section1D.h"


namespace RtoR::Thermal {

    class Monitor : public RtoR::Monitor {
        Real transientGuess = -1.0;

        Real u=.0;

        Real barϕ=.0;
        Real tau=.0;
        Real tau_indirect=.0;

        Real Δt = 0.0;

        Spaces::PointSet temperature1HistoryData;
        Spaces::PointSet temperature2HistoryData;
        Spaces::PointSet temperature3HistoryData;

        Base::Graphics::Graph2D mTemperaturesGraph;

        Spaces::PointSet histogramKData;
        Spaces::PointSet histogramGradData;
        Spaces::PointSet histogramVData;
        Spaces::PointSet histogramEData;

        Base::Graphics::Graph2D mHistogramsGraphK;
        Base::Graphics::Graph2D mHistogramsGraphGrad;
        Base::Graphics::Graph2D mHistogramsGraphV;
        Base::Graphics::Graph2D mHistogramsGraphE;

        RtoR2::StraightLine corrSampleLine;
        std::shared_ptr<R2toR::Sampler> sampler;
        R2toR::CorrelationFunction mCorrelationFunction;
        RtoR::Section1D::Ptr mSpaceCorrelation;
        GraphRtoR mCorrelationGraph;
        RtoR::Graphics::HistoryDisplay mFullHistoryDisplay;

    protected:
        void handleOutput(const OutputPacket &outInfo) override;

    public:
        explicit Monitor(const NumericConfig &, KGEnergy &hamiltonian);

        void draw() override;

        void setTransientGuess(Real guess);

        void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory) override;
    };
}




#endif //STUDIOSLAB_THERMALMONITOR_H
