//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_GRAPH_H
#define V_SHAPE_GRAPH_H


#include "Studios/Graphics/PlottingUtils.h"
#include "Studios/Graphics/Artists/Artist.h"

#include "Common/Workaround/StringStream.h"
#include "Common/BinaryToInt.h"

#include "Studios/Graphics/WindowManagement/Window.h"

#include "Studios/Graphics/Artists/StylesAndColorSchemes.h"
#include "Studios/Graphics/OpenGL/Utils.h"


template<class FunctionType>
class Graph : public Artist {
    typedef std::tuple<const FunctionType*, Color, String> FunctionTriple;


    std::vector<FunctionTriple> mFunctions;

    void _drawAxes(const Window *win);
    void __drawXAxis(const Window *win);
    void __drawYAxis(const Window *win);

protected:
    String title = "";
    bool filled = false;
    int samples = 512;

    double yspacing = 1.e-5;



public:
    double xMin, xMax, yMin, yMax;

    Graph(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
               String title = "no_title", bool filled = false, int samples = 512);

    /*!
     * Draw presumes OpenGL model view matrix is identity.
     */
    void draw(const Window *window) override;

    void setupOrtho();

    void addFunction(const FunctionType* func, Color color=ColorScheme::defaultGraphColor, String name="");
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

void auxRectDraw(int i, Color color, String label, const Window *window);

template<class FunctionType>
void Graph<FunctionType>::setupOrtho()
{
    glMatrixMode(GL_PROJECTION);
    const double deltaX = xMax-xMin;
    const double deltaY = yMax-yMin;
    const double xTraLeft  = -deltaX*0.07;
    const double xTraRight = +deltaX*0.02;

    glOrtho(xMin+xTraLeft, xMax+xTraRight,
    (yMin-deltaY*0.025), (yMax+deltaY*0.025), -1, 1);
}

template<class FunctionType>
void Graph<FunctionType>::draw(const Window *window) {

    setupOrtho();

    glMatrixMode(GL_MODELVIEW);

    auto &tf = ColorScheme::graphTitleFont;
    glColor4f(tf.r, tf.g, tf.b, tf.a);
    //writeOrtho(window, {xMin, xMax, yMin, yMax}, 2, -0.95, 0.85, title, FONT_STROKE_ROMAN);

    _drawAxes(window);


    int i=0;
    for(auto &triple : mFunctions){
        auto &func = *std::get<0>(triple);
        auto color = std::get<1>(triple);
        auto label = std::get<2>(triple);

        auxRectDraw(i++, color, label, window);

        this->_renderFunction(&func, color);
    }


}

template<class FunctionType>
void Graph<FunctionType>::_drawAxes(const Window *win) {
    double deltaY;
    double markStart = yMax;

    {
        deltaY = yMax-yMin;


        while(markStart > yMin)
            markStart-=yspacing;
        while(markStart < (yMin-yspacing))
            markStart+=yspacing;

        yspacing = deltaY/10;
    }

    glLineWidth(1.0);

    __drawXAxis(win);
    __drawYAxis(win);

    if(!labels.empty()){
        const double Sx = (xMax-xMin) / win->w;
        const double Sy = (yMax-yMin) / win->h;
        const double Tx = xMin;
        const double Ty = yMin;

        for(size_t i=0; i<labels.size(); ++i){
            labels[i]->draw(Sx, Sy, Tx, Ty);
        }
    }
}

template<class FunctionType>
void Graph<FunctionType>::__drawXAxis(const Window *win) {
    const double inPixelsTimes2 = 5;
    const double vTick = inPixelsTimes2 * (yMax-yMin) / win->h;
    const double hTick = inPixelsTimes2 * (xMax-xMin) / win->w;
    (void)hTick;

    auto &gtf = ColorScheme::graphTicksFont;

    double xspacing = (xMax-xMin) / 10.0;
    if(xspacing == .0) xspacing = 1.0;

    {
        glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);

        const double yloc = -yspacing*0.356;
        for (double mark = 0; mark <= xMax * 1.0001; mark += xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            writeOrtho(win, {xMin, xMax, yMin, yMax}, 1, mark - xspacing / 18.0, yloc, buffer, TICK_FONT);
        }
        for (double mark = 0; mark >= xMin * 1.0001; mark -= xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            writeOrtho(win, {xMin, xMax, yMin, yMax}, 1, mark - xspacing / 18.0, yloc, buffer, TICK_FONT);
        }
    }

    if(1)
    {
        auto &ac = ColorScheme::axisColor;
        auto &tc = ColorScheme::tickColor;
        glBegin(GL_LINES);
        {
            //glPopAttrib();

            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(xMin, 0, 0);
            glVertex3d(xMax, 0, 0);

            glColor4f(tc.r, tc.g, tc.b, tc.a);

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
void Graph<FunctionType>::__drawYAxis(const Window *win) {
    const double deltaY = yMax-yMin;
    const double deltaX = xMax-xMin;
    const double xloc = xMin-deltaX*0.05;

    auto markStart = yMin;

    StringStream buffer;
    //buffer << std::scientific << std::setprecision(2);

    auto &gtf = ColorScheme::graphTicksFont;
    glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
    {
        for(double mark = markStart; mark<=yMax; mark+=yspacing)
        {
            buffer.str("");
            if(fabs(mark) > deltaY*1.e-15)
                buffer << mark;
            else
                buffer << "0";
            writeOrtho(win, {xMin,xMax,yMin,yMax}, 1, float(xloc), float(mark), buffer.str().c_str(), TICK_FONT);
        }
    }


    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x2727);
    glLineStipple(2, 0x1249);
    glLineStipple(2, 0x1111);

    glBegin(GL_LINES);
    {
        auto &ac = ColorScheme::axisColor;
        auto &tc = ColorScheme::tickColor;

        glColor4f(tc.r, tc.g, tc.b, tc.a);

        for(double mark = markStart; mark<=yMax; mark+=yspacing ){
            glVertex3d(xMin, mark, 0);
            glVertex3d(xMax, mark, 0);
        }

        glColor4f(ac.r, ac.g, ac.b, ac.a);

        glVertex3d(xMin, 0, 0);
        glVertex3d(xMax, 0, 0);
    }
    glEnd();
    glPopAttrib();
}

template<class FunctionType>
void Graph<FunctionType>::addFunction(const FunctionType *func, Color color, String name) {
    mFunctions.emplace_back(FunctionTriple{func, color, name});
}

template<class FunctionType>
void Graph<FunctionType>::clearFunctions() {
    mFunctions.clear();
}


#endif //V_SHAPE_GRAPH_H
