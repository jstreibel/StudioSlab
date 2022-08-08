//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_GRAPH_H
#define V_SHAPE_GRAPH_H


#include "Studios/Graphics/PlottingUtils.h"
#include "Studios/Graphics/WindowManagement/Artist.h"

#include <Fields/Mappings/RtoR/Model/RtoRFunction.h>



class Graph : public Artist {
    std::vector<std::pair<const RtoR::Function*, Color>> mFunctions;

    double xMin=-1, xMax=1, yMin=-1, yMax=1;
    String title = "";
    bool filled = false;
    int samples = 512;

    double yspacing = 1.e-5;

    void _drawAxes(int winW, int winH);
    void __drawXAxis(int winW, int winH);
    void __drawYAxis(int winW, int winH);
public:

    Graph();
    Graph(double xMin, double xMax, double yMin, double yMax,
               String title = "", bool filled = false, int samples = 512);

    /*!
     * Draw presumes OpenGL model view matrix is identity.
     */
    void draw(const Window *window) override;


public:
    void addLabel(Label *label)
    {
        labels.push_back(label);
    }
private:
    std::vector<Label*> labels;

};


#endif //V_SHAPE_GRAPH_H
