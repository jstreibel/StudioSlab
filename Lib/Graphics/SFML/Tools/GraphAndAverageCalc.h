//
// Created by joao on 20/05/2021.
//

#ifndef ISING_GRAPHANDAVERAGECALC_H
#define ISING_GRAPHANDAVERAGECALC_H

#include "../Graph.h"

namespace Slab::Lost::ThermoOutput {

    typedef Pair<double,double> DoublePair;
    typedef Vector<Pair<DoublePair ,DoublePair>> AveragesHistory;

    class GraphAndAverageCalc : public Lost::Graph {

        sf::VertexArray postTransientDataAvgPlusSigma;
        sf::VertexArray postTransientDataAvg;
        sf::VertexArray postTransientDataAvgMinusSigma;

        sf::VertexArray lastManipTimeVerticalLine;

        int lastSimTime_of_UserManipulation_of_SystemParameters = 0;
        int transientTime;

        sf::VertexArray movingTransientVerticalLine;

        Lost::Graph *avgSubgraph = nullptr;
        Lost::Graph *sigmaSubgraph = nullptr;
        bool avgAbs;

        double avg;
        double avg2;
        bool firstRound = true;

        bool computeAverages=true;
    public:
        GraphAndAverageCalc(sf::IntRect subWindow,
                            sf::Vector2f rMin,
                            sf::Vector2f rMax,
                            unsigned MCSteps,
                            unsigned transient,
                            std::string xAxisLabel, std::string yAxisLabel, bool avgAbs);

        void addPoint(float x, float y, sf::Color color, bool rescaleIfOverflow=false) override;

        /**
         *
         * @param previousParameterValue Temperatura (T) ou campo magnetico externo (h)
         */
        virtual void manipulationOfParametersHasHappened(double previousParameterValue, double sigmaFactor) override;

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        void setAvgOutputSubGraph(Lost::Graph *avgSubgraph, Lost::Graph *sigmaSubgraph) {
            this->avgSubgraph = avgSubgraph;
            this->sigmaSubgraph = sigmaSubgraph;
        };

        AveragesHistory getAveragesHistory();

        void toggleComputeAverages();
    };
}

#endif //ISING_GRAPHANDAVERAGECALC_H
