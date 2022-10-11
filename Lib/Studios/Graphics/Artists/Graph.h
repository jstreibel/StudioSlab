//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_GRAPH_H
#define V_SHAPE_GRAPH_H


#include "Studios/Graphics/PlottingUtils.h"
#include "Studios/Graphics/Artists/Artist.h"

#include "Common/Workaround/StringStream.h"

#include "Studios/Graphics/WindowManagement/Window.h"


#ifndef TITLE_FONT
#define TITLE_FONT FONT9
#endif
#ifndef TICK_FONT
#define TICK_FONT FONT9
#endif



template<class FunctionType>
class Graph : public Artist {
    typedef std::pair<const FunctionType*, Color> FuncColorPair;

    std::vector<FuncColorPair> mFunctions;

    void _drawAxes(int winW, int winH);
    void __drawXAxis(int winW, int winH);
    void __drawYAxis(int winW, int winH);

protected:
    String title = "";
    bool filled = false;
    int samples = 512;

    double yspacing = 1.e-5;



public:
    double xMin, xMax, yMin, yMax;

    Graph(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
               String title = "", bool filled = false, int samples = 512);

    /*!
     * Draw presumes OpenGL model view matrix is identity.
     */
    void draw(const Window *window) override;

    void addFunction(const FunctionType* func, Color color={1,1,1,1});
    void clearFunctions();

    void addLabel(Label *label)
    {
        labels.push_back(label);
    }

private:
    std::vector<Label*> labels;

protected:

    virtual void _renderFunction(const FunctionType *func, Color color) = 0;

};



template<class FunctionType>
Graph<FunctionType>::Graph(double xMin, double xMax, double yMin, double yMax, String title, bool filled, int samples)
        : xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), title(title), filled(filled), samples(samples)
{

}

template<class FunctionType>
void Graph<FunctionType>::draw(const Window *window) {
    glMatrixMode(GL_MODELVIEW);

    write(-0.95, 0.85, title, TITLE_FONT);

    {
        const double deltaX = xMax-xMin;
        const double deltaY = yMax-yMin;
        const double xTraLeft  = -deltaX*0.07;
        const double xTraRight = +deltaX*0.02;

        glOrtho(xMin+xTraLeft, xMax+xTraRight,
                (yMin-deltaY*0.025), (yMax+deltaY*0.025), -1, 1);
    }

    _drawAxes(window->w, window->h);

    for(auto &pair : mFunctions){
        auto &func = *pair.first;
        auto color = pair.second;

        this->_renderFunction(&func, color);
    }


}

template<class FunctionType>
void Graph<FunctionType>::_drawAxes(int winW, int winH) {
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
    //__drawYAxis(winW, winH);

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

template<class FunctionType>
void Graph<FunctionType>::__drawXAxis(int winW, int winH) {
    const double inPixelsTimes2 = 5;
    const double vTick = inPixelsTimes2 * (yMax-yMin) / winH;
    const double hTick = inPixelsTimes2 * (xMax-xMin) / winW;
    (void)hTick;

    double xspacing = (xMax-xMin) / 10.0;
    if(xspacing == .0) xspacing = 1.0;

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

    if(1)
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

template<class FunctionType>
void Graph<FunctionType>::__drawYAxis(int winW, int winH) {
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

template<class FunctionType>
void Graph<FunctionType>::addFunction(const FunctionType *func, Color color) {
    mFunctions.emplace_back(FuncColorPair{func, color});
}

template<class FunctionType>
void Graph<FunctionType>::clearFunctions() {
    mFunctions.clear();
}


#endif //V_SHAPE_GRAPH_H
