/*
 * plotting.h
 *
 *  Created on: 21 de set de 2017
 *      Author: johnny
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include "Core/Util/Workaround/STDLibInclude.h"
#include "Controller/Utils/ParameterMaps.h"
#include "View/OpenGL/Tools/plotting-utils.h"
#include "View/OpenGL/Tools/WindowManagement/Window.h"
#include "Artist_depr.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <RtoR/Model/RtoRFunction.h>
#include <RtoR/Model/RtoRFunctionArbitrary.h>

struct Graph_depr : Artist_depr
{
    std::vector<std::pair<const RtoR::Function*, Color>> mFunctions;

    Graph_depr();
    Graph_depr(Window window,
          const double xMin, const double xMax,
          const double yMin, const double yMax,
          String title = "", bool filled = false, int resolution = 512);

    ~Graph_depr();

    void draw() override;

    void BindWindow(bool clearAndDrawDecor=true);

    void drawYAxis();
    static void drawXAxis(const double xMin, const double xMax, const double yspacing);
    void drawXAxis();

    void DrawAxes(double deltaY = -1);

    double yspacing;
    double markStart;

    double yMin, yMax;
    double xMin, xMax;

    String title;

    bool filled;
    int resolution;

    bool drawXLabels = true;
    bool drawYLabels = true;

    Window myWindow;

    std::vector<Label*> labels;

    void addLabel(Label *label)
    {
        labels.push_back(label);
    }
};

#endif /* GRAPH_H_ */
