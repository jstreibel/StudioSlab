//
// Created by joao on 18/05/2021.
//

#include "Graph.h"
#include "../../Utils/ViewerUtils.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>


using namespace ThermoOutput;


Graph::Graph(sf::IntRect subWindow, sf::Vector2f rMin, sf::Vector2f rMax,
              std::string xAxisLabel, std::string yAxisLabel)
: theData(sf::LinesStrip), scale(scale), rMin(rMin), rMax(rMax) {
    setPlacing(subWindow);

    ViewerUtils::LoadFont(font);

    int fontSize = 14;

    sf::Text text = sf::Text(yAxisLabel, font, fontSize);
    float yMid = mySubWindow.height*(1./2);
    auto tBounds = text.getGlobalBounds();
    auto textWidth = tBounds.width;
    auto textHeight = tBounds.height;
    text.setPosition(-(fontSize+2),yMid-textHeight/2);
    text.setOrigin(textWidth/2, textHeight/2);
    text.setRotation(-90);
    labels.push_back(text);

    text = sf::Text(xAxisLabel, font, fontSize);
    float xMid = mySubWindow.width/2;
    float yBelow = mySubWindow.height +1;
    text.setPosition(xMid, yBelow);
    labels.push_back(text);
}




void Graph::setPlacing(sf::IntRect placing) {
    this->mySubWindow = placing;
    _computeScale();

    {
        int sizeHint = theData.getVertexCount();
        for (auto i = 0; i < sizeHint; ++i)
            theData[i].color = sf::Color::White;
    }


    {
        frame.setPrimitiveType(sf::Lines);

        float w = placing.width, h = placing.height;
        frame.append({{0, 0}, sf::Color::White});
        frame.append({{w, 0}, sf::Color::White});
        frame.append({{w, 0}, sf::Color::White});
        frame.append({{w, h}, sf::Color::White});
        frame.append({{w, h}, sf::Color::White});
        frame.append({{0, h}, sf::Color::White});
        frame.append({{0, h}, sf::Color::White});
        frame.append({{0, 0}, sf::Color::White});
    }

    // Axes
    {
        axes.setPrimitiveType(sf::Lines);

        float w=rMax.x-rMin.x, h=rMax.y-rMin.y;
        const int tickCount = 10;
        for(int i=0; i < tickCount; ++i){
            axes.append({{(float(i) / tickCount) * w + rMin.x, -h / 80}, sf::Color::White});
            axes.append({{(float(i) / tickCount) * w + rMin.x, h / 80}, sf::Color::White});
        }
        axes.append({{rMin.x, 0}, sf::Color::White});
        axes.append({{rMax.x, 0}, sf::Color::White});
    }
}


void Graph::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform.translate(mySubWindow.left, mySubWindow.top);
    target.draw(&frame[0], frame.getVertexCount(), frame.getPrimitiveType(), states);

    for(const auto &label : labels)
        target.draw(label, states);


    states.transform.translate(0, mySubWindow.height);
    states.transform.scale(scale);
    states.transform.translate(-rMin.x, -rMin.y);

    target.draw(&axes[0], axes.getVertexCount(), axes.getPrimitiveType(), states);

    target.draw(&theData[0], theData.getVertexCount(), theData.getPrimitiveType(), states);
}


void Graph::addPoint(float x, float y, sf::Color color) {
    theData.append({{x, y}, color});

    if(y>rMax.y){
        rMax.y = y>0?y*1.1:y/1.1;
        _computeScale();
    }
    else if(y<rMin.y){
        rMin.y = y>0?y/1.1:y*1.1;
        _computeScale();
    }
    if(x>rMax.x){
        rMax.x = x>0?x*1.1:x/1.1;
        _computeScale();
    }
    else if(x<rMin.x){
        rMax.x = x>0?x/1.1:x*1.1;
        _computeScale();
    }
}

void Graph::_computeScale() {
    scale.x = mySubWindow.width / (rMax.x - rMin.x);
    scale.y = -mySubWindow.height / (rMax.y - rMin.y);
}

void Graph::addHorizontalDashedLine(float y, int dashCount, sf::Color color) {
    float w=rMax.x-rMin.x, h=rMax.y-rMin.y;

    for(int i=0; i<dashCount; i++){
        float x0 = rMin.x + w*(float(i  ) / dashCount),
                x1 = rMin.x + w*(float(i + 1) / dashCount);
        axes.append({{x0, y}, color});
        axes.append({{x1, y}, color});

        ++i;
    }
}

void Graph::addVerticalDashedLine(float x, int dashCount, sf::Color color) {
    float h=rMax.y-rMin.y;

    for(int i=0; i<dashCount; i++){
        float y0 = rMin.y + h*(float(i  ) / dashCount),
              y1 = rMin.y + h*(float(i + 1) / dashCount);
        axes.append({{x, y0}, color});
        axes.append({{x, y1}, color});

        ++i;
    }
}

void Graph::manipulationOfParametersHasHappened(double previousParamValue, double sigmaFactor) {
    // esse metodo (essa funcao) eh um presente pras subclasses dessa classe
}

std::vector<std::pair<double,double>> Graph::getData() const {
    std::vector<std::pair<double,double>> data(theData.getVertexCount());
    for(int i=0; i<theData.getVertexCount(); i++){
        auto iData = theData[i].position;
        data[i] = {iData.x, iData.y};
    }

    return data;
}

void Graph::clearData() {
    theData.clear();
}






