//
// Created by joao on 20/05/2021.
//

#ifndef ISING_GRAPHANDAVERAGECALC_H
#define ISING_GRAPHANDAVERAGECALC_H

#include "Graph.h"

namespace ThermoOutput {

    typedef std::pair<double,double> DoublePair;
    typedef std::vector<std::pair<DoublePair ,DoublePair>> AveragesHistory;

    class GraphAndAverageCalc : public Graph {

        sf::VertexArray postTransientDataAvgPlusSigma;
        sf::VertexArray postTransientDataAvg;
        sf::VertexArray postTransientDataAvgMinusSigma;

        sf::VertexArray lastManipTimeVerticalLine;

        int lastSimTime_of_UserManipulation_of_SystemParameters = 0;
        int transientTime;

        sf::VertexArray movingTransientVerticalLine;

        Graph *avgSubgraph = nullptr;
        Graph *sigmaSubgraph = nullptr;
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

        void addPoint(float x, float y, sf::Color color) override;

        /**
         *
         * @param previousParameterValue Temperatura (T) ou campo magnetico externo (h)
         */
        virtual void manipulationOfParametersHasHappened(double previousParameterValue, double sigmaFactor) override;

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        void setAvgOutputSubGraph(Graph *avgSubgraph, Graph *sigmaSubgraph) {
            this->avgSubgraph = avgSubgraph;
            this->sigmaSubgraph = sigmaSubgraph;
        };

        AveragesHistory getAveragesHistory();

        void toggleComputeAverages();
    };
}

#endif //ISING_GRAPHANDAVERAGECALC_H
