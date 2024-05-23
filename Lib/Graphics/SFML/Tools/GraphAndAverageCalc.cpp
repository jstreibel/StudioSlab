//
// Created by joao on 20/05/2021.
//

#include "GraphAndAverageCalc.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>


namespace Slab::Lost::ThermoOutput {


    GraphAndAverageCalc::GraphAndAverageCalc(sf::IntRect subWindow,
                                             sf::Vector2f rMin,
                                             sf::Vector2f rMax,
                                             unsigned MCSteps,
                                             unsigned transient,
                                             std::string xAxisLabel, std::string yAxisLabel, bool avgAbs)
            : Lost::Graph(subWindow, rMin, rMax, xAxisLabel, yAxisLabel), transientTime(transient) {
        this->avgAbs = avgAbs;

        u_char gray = 80;
        sf::Color color(gray, gray, gray);
        addVerticalDashedLine(transient, 70, color);

        // Cria a linha do tempo atual menos o transiente
        {
            float w = rMax.x - rMin.x, h = rMax.y - rMin.y;
            int dashCount = 120;
            for (int i = 0; i < dashCount; i++) {
                float y0 = rMin.y + h * (float(i) / dashCount),
                        y1 = rMin.y + h * (float(i + 1) / dashCount);
                movingTransientVerticalLine.append({{0, y0}, color});
                movingTransientVerticalLine.append({{0, y1}, color});

                ++i;
            }
        }

        // Cria a linha que mostra a ultima manipulacao de parametros pelo usuario
        {
            float w = rMax.x - rMin.x, h = rMax.y - rMin.y;
            int dashCount = 120;
            for (int i = 0; i < dashCount; i++) {
                float y0 = rMin.y + h * (float(i) / dashCount),
                        y1 = rMin.y + h * (float(i + 1) / dashCount);
                lastManipTimeVerticalLine.append({{0, y0}, sf::Color::Red});
                lastManipTimeVerticalLine.append({{0, y1}, sf::Color::Red});

                ++i;
            }
        }
    }


    void GraphAndAverageCalc::addPoint(float x, float y, sf::Color color, bool rescaleIfOverflow) {

        Lost::Graph::addPoint(x, y, color, rescaleIfOverflow);

        auto currentTime = (int) theData.getVertexCount();

        const auto lastManip = lastSimTime_of_UserManipulation_of_SystemParameters;
        if (currentTime - transientTime > lastManip && computeAverages) {
            const auto totalSamples = currentTime - (lastManip + transientTime);
            auto avg = 0.0;
            auto avg2 = 0.0;
            const auto start = lastManip + transientTime, end = currentTime;
            for (int i = start; i < end; ++i) {
                auto val = theData[i].position.y;
                avg += val;
                avg2 += val * val;
            }

            avg /= totalSamples;
            avg2 /= totalSamples;
            const auto sigma = sqrt(avg2 - avg * avg);

            auto color = sf::Color(255, 0, 0, 255);
            auto colorFaded = color;
            colorFaded.a = 0.1 * 255;

            postTransientDataAvgPlusSigma.append({{x, float(avg + sigma)}, colorFaded});
            postTransientDataAvg.append({{x, (float) avg}, color});
            postTransientDataAvgPlusSigma.append({{x, float(avg - sigma)}, colorFaded});

            this->avg = avg;
            this->avg2 = avg2;
        }
    }

    void GraphAndAverageCalc::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        Lost::Graph::draw(target, states);

        states.transform.translate(mySubWindow.left, mySubWindow.top);

        states.transform.translate(0, mySubWindow.height);
        states.transform.scale(scale);
        states.transform.translate(-rMin.x, -rMin.y);

        target.draw(&movingTransientVerticalLine[0], movingTransientVerticalLine.getVertexCount(),
                    movingTransientVerticalLine.getPrimitiveType(), states);
        target.draw(&lastManipTimeVerticalLine[0], lastManipTimeVerticalLine.getVertexCount(),
                    lastManipTimeVerticalLine.getPrimitiveType(), states);

        target.draw(&postTransientDataAvgPlusSigma[0], postTransientDataAvgPlusSigma.getVertexCount(), sf::Points,
                    states);
        target.draw(&postTransientDataAvg[0], postTransientDataAvg.getVertexCount(), sf::Points, states);
        target.draw(&postTransientDataAvgMinusSigma[0], postTransientDataAvgMinusSigma.getVertexCount(), sf::Points,
                    states);

    }

    void GraphAndAverageCalc::manipulationOfParametersHasHappened(double previousParameterValue, double sigmaFactor) {
        lastSimTime_of_UserManipulation_of_SystemParameters = theData.getVertexCount();

        for (auto i = 0; i < lastManipTimeVerticalLine.getVertexCount(); ++i)
            lastManipTimeVerticalLine[i].position.x = (float) lastSimTime_of_UserManipulation_of_SystemParameters;

        addVerticalDashedLine((float) lastSimTime_of_UserManipulation_of_SystemParameters, 1,
                              sf::Color(255, 0, 0, 255 / 5));

        if (firstRound) {
            firstRound = false;
            return;
            // Gambiarra pra resolver um bug.
        }

        if (avgAbs) avg = fabs(avg);
        avgSubgraph->addPoint(previousParameterValue, avg, sf::Color(255, 255, 255, 128));
        const Real sigma2 = sigmaFactor * (avg2 - avg * avg);
        sigmaSubgraph->addPoint(previousParameterValue, sigma2, sf::Color(255, 255, 255, 128));

        //sigmaSubgraph
    }

    AveragesHistory GraphAndAverageCalc::getAveragesHistory() {
        AveragesHistory data(postTransientDataAvg.getVertexCount());

        for (int i = 0; i < postTransientDataAvg.getVertexCount(); i++) {
            auto avg = postTransientDataAvg[i].position;
            auto avgPlusSigma = postTransientDataAvgPlusSigma[i].position;
            auto avgMinusSigma = postTransientDataAvgMinusSigma[i].position;

            data[i] = {{avg.x,          avg.y},
                       {avgPlusSigma.y, avgMinusSigma.y}};
        }

        return data;
    }

    void GraphAndAverageCalc::toggleComputeAverages() {
        computeAverages = !computeAverages;
    }


}