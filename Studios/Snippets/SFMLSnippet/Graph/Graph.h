//
// Created by joao on 18/05/2021.
//

#ifndef ISING_GRAPH_H
#define ISING_GRAPH_H

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include "InputOutput.h"


namespace ThermoOutput {


    class Graph : public sf::Drawable {
    protected:
        sf::VertexArray frame;
        sf::VertexArray axes;

        sf::VertexArray theData;

        sf::IntRect mySubWindow;
        sf::Vector2f scale;
        sf::Vector2f rMin, rMax;

        sf::Font font;
        std::vector<sf::Text> labels;


        void _computeScale();

    public:
        Graph(sf::IntRect subWindow,
              sf::Vector2f rMin,
              sf::Vector2f rMax,
              unsigned MCSteps,
              unsigned transient,
              std::string xAxisLabel, std::string yAxisLabel);

        void setPlacing(sf::IntRect placing);

        virtual void addPoint(float x, float y, sf::Color color = sf::Color::White);

        void addHorizontalDashedLine(float y, int dashCount = 200, sf::Color color = sf::Color::White);

        void addVerticalDashedLine(float x, int dashCount = 200, sf::Color color = sf::Color::White);

    protected:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    public:
        virtual void manipulationOfParametersHasHappened(double previousParamValue, double sigmaFactor);

        std::vector<std::pair<double,double>> getData() const;
    };
}


#endif //ISING_GRAPH_H
