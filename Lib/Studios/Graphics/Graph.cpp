//
// Created by joao on 03/09/2021.
//

#include "Graph.h"

#include <Common/Workaround/StringStream.h>

#include "Studios/Graphics/WindowManagement/Window.h"
#include "Fields/Mappings/FunctionRenderer.h"


#define TITLE_FONT FONT9
#define TICK_FONT FONT9


Graph::Graph() = default;


Graph::Graph(double xMin, double xMax, double yMin, double yMax, String title, bool filled, int samples)
    : xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), title(title), filled(filled), samples(samples)
{

}


void Graph::draw(const Window *window) {
    glMatrixMode(GL_MODELVIEW);

    write(-0.95, 0.85, title, TITLE_FONT);

    {
        const double deltaX = xMax-xMin;
        const double deltaY = yMax-yMin;
        const double xTraLeft  = -deltaX*0.07;
        const double xTraRight = +deltaX*0.02;

        glOrtho(xMin+xTraLeft, xMax+xTraRight, (yMin-deltaY*0.025), (yMax+deltaY*0.025), -1, 1);
    }

    _drawAxes(window->w, window->h);

    for(auto &pair : mFunctions){
        auto &func = *pair.first;
        auto color = pair.second;

        RtoR::FunctionRenderer::renderFunction(func, color, filled, xMin, xMax, samples);
    }


}

void Graph::_drawAxes(int winW, int winH) {
    double deltaY;
    double markStart = yMax;

    {
        deltaY = yMax-yMin;

        while(markStart > yMin)
            markStart-=yspacing;
        while(markStart < (yMin-yspacing))
            markStart+=yspacing;

        if(deltaY/yspacing < 5) yspacing *= 0.5;
        if(deltaY/yspacing > 10) yspacing *= 2.0;
    }

    __drawXAxis(winW, winH);
    __drawYAxis(winW, winH);

    if(!labels.empty()){
        const double Sx = (xMax-xMin) / winW;
        const double Sy = (yMax-yMin) / winH;
        const double Tx = xMin;
        const double Ty = yMin;

        for(size_t i=0; i<labels.size(); ++i){
            labels[i]->draw(Sx, Sy, Tx, Ty);
        }

        /*
        const double inPixelsTimes2 = 15;
        const double vTick = inPixelsTimes2 * Sy;
        const double hTick = inPixelsTimes2 * Sx;
        glColor3f(0,0,0);
        glBegin(GL_LINES);
        for(size_t i=0; i<labels.size(); ++i){
            const Label *l = labels[i];
            const Point2D &loc = l->pos;

            double x = loc.x, y = loc.y;
            if(l->isPosAbsolute){
                x = (x+xMin)*Sx;
                y = (y+yMin)*Sy;
            }

            glVertex2f(x, y);
            glVertex2f(x+hTick, y+vTick);
        }
        glEnd();

        for(size_t i=0; i<labels.size(); ++i){
            const Label *l = labels[i];
            const Point2D &loc = l->pos;

            double x = loc.x, y = loc.y;
            if(l->isPosAbsolute){
                x = (x+xMin)*Sx;
                y = (y+yMin)*Sy;
            }

            write(x+hTick, y+vTick, l->getText());
        }
        */

    }
}

void Graph::__drawXAxis(int winW, int winH) {
    const double inPixelsTimes2 = 5;
    const double vTick = inPixelsTimes2 * (yMax-yMin) / winH;
    const double hTick = inPixelsTimes2 * (xMax-xMin) / winW;
    (void)hTick;

    const double xspacing = (xMax-xMin) / 10.0;

    //if(drawXLabels)
    {
        const double yloc = -yspacing*0.20;
        for(double mark = 0; mark<=xMax*1.0001; mark+=xspacing)
        {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            glColor4f(.4f, .4f, 0.5f, 0.6f);
            write(mark-xspacing/18.0, yloc, buffer, TICK_FONT);
        }
        for(double mark = 0; mark>=xMin*1.0001; mark-=xspacing)
        {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            glColor4f(.4f, .4f, 0.5f, 0.6f);
            write(mark-xspacing/18.0, yloc, buffer, TICK_FONT);
        }
    }

    //if(drawXTicks)
    {
        glBegin(GL_LINES);
        {
            //glPopAttrib();

            glColor3f(.9f, .9f, .9f);

            glVertex3d(xMin, 0, 0);
            glVertex3d(xMax, 0, 0);

            glColor4f(.0f, .0f, 0.0f, 1.0f);

            for(double mark = 0; mark<=xMax; mark+=xspacing){
                glVertex3d(mark, -vTick, 0);
                glVertex3d(mark, +vTick, 0);
            }
            for(double mark = 0; mark>=xMin; mark-=xspacing){
                glVertex3d(mark, -vTick, 0);
                glVertex3d(mark, +vTick, 0);
            }
        }
        glEnd();
    }
}

void Graph::__drawYAxis(int winW, int winH) {
    const double deltaY = yMax-yMin;
    const double deltaX = xMax-xMin;
    const double xloc = xMin-deltaX*0.05;

    auto markStart = yMin;

    StringStream buffer;
    //buffer << std::scientific << std::setprecision(2);

    //if(drawYLabels)
    {
        for(double mark = markStart; mark<=yMax; mark+=yspacing)
        {
            buffer.str("");
            if(fabs(mark) > deltaY*1.e-15)
                buffer << mark;
            else
                buffer << "0";
            glColor4f(.4f, .4f, 0.5f, 0.6f);
            write(float(xloc), float(mark), buffer.str().c_str(), TICK_FONT);
        }
    }

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    {
        glColor4f(.8f, .8f, 1.0f, 0.7f);

        glLineStipple(2, 0x00FF);
        for(double mark = markStart; mark<=yMax; mark+=yspacing ){
            glVertex3d(xMin, mark, 0);
            glVertex3d(xMax, mark, 0);
        }

        glColor3f(.9f, .9f, .9f);

        glVertex3d(xMin, 0, 0);
        glVertex3d(xMax, 0, 0);
    }
    glEnd();
    glPopAttrib();
}



