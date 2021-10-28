/*
 * plotting.h
 *
 *  Created on: 21 de set de 2017
 *      Author: johnny
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include "PlottingUtils.h"

#include <Studios/Util/STDLibInclude.h>

#include <GL/gl.h>
#include <GL/glut.h>

struct Graph
{
    Graph();
    Graph(const double winX, const double winY,
          const double winW, const double winH,
          const double xMin, const double xMax,
          const double yMin, const double yMax,
          String title = "");

    ~Graph();

    void BindWindow(bool clearAndDrawDecor=true);

    void drawYAxis();
    static void drawXAxis(const double xMin, const double xMax, const double yspacing);
    void drawXAxis();

    void DrawAxes(double deltaY = -1);

    double yspacing;
    double markStart;

    double winXoffset = 2;
    double winYoffset = 2;

    //AARect2D windowRect;
    //AARect2D graphRect;

    double winX, winY;
    double winW, winH;
    double yMin, yMax;
    double xMin, xMax;

    String title;

    bool drawXLabels = true;
    bool drawYLabels = true;

private:
    std::vector<Label*> labels;
public:
    void addLabel(Label *label)
    {
        labels.push_back(label);
    }
};

#endif /* GRAPH_H_ */
