//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_GRAPH_H
#define V_SHAPE_GRAPH_H


#include "Studios/Graphics/PlottingUtils.h"
#include "Studios/Graphics/Artists/Artist.h"

#include "Fields/Mappings/RtoR/Model/RtoRFunction.h"



class Graph : public Artist {
    typedef std::pair<const RtoR::Function*, Color> FuncColorPair;

    std::vector<FuncColorPair> mFunctions;

    String title = "";
    bool filled = false;
    int samples = 512;

    double yspacing = 1.e-5;

    void _drawAxes(int winW, int winH);
    void __drawXAxis(int winW, int winH);
    void __drawYAxis(int winW, int winH);


public:
    double xMin, xMax, yMin, yMax;

    Graph(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
               String title = "", bool filled = false, int samples = 512);

    /*!
     * Draw presumes OpenGL model view matrix is identity.
     */
    void draw(const Window *window) override;

    void addFunction(const RtoR::Function* func, Color color);
    void clearFunctions();


    void addLabel(Label *label)
    {
        labels.push_back(label);
    }

private:
    std::vector<Label*> labels;

};


#endif //V_SHAPE_GRAPH_H
