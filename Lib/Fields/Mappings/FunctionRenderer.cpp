//

#include <Phys/Numerics/Allocator.h>
#include "FunctionRenderer.h"
#include "Base/Graphics/Styles/StylesAndColorSchemes.h"


//
// Created by joao on 27/09/2019.

void RtoR::FunctionRenderer::renderFunction(const RtoR::ArbitraryFunction &func, Styles::Color c, bool filled, Real scale){
    auto &params = Numerics::Allocator::getInstance().getNumericParams();

    const Real xMin = params.getxLeft(),
               xMax = xMin + params.getL();
    const int N = params.getN();

    renderFunction(func, c, filled, xMin, xMax, N, scale);
}

void RtoR::FunctionRenderer::renderFunction(const RtoR::Function &func, Styles::Color c, bool filled, Real xMin, Real xMax, PosInt resolution, Real scale) {
    const double dx = (xMax-xMin) / double(resolution);
    const double xBegin = xMin;
    const double xEnd = xMax;

    if(dx == .0) return;

    if(filled)
    {
        glColor4f(c.r, c.g, c.b, c.a/3.0);
        glBegin(GL_QUADS);
        {
            for(double x=xBegin; x<xEnd; x+=dx){
                const double xmin = x;
                const double xmax = x+dx;

                const double ymin = 0,
                        ymax1 = scale*func(xmin),
                        ymax2 = scale*func(xmax);

                glVertex2d(xmin, ymin);
                glVertex2d(xmin, ymax1);
                glVertex2d(xmax, ymax2);
                glVertex2d(xmax, ymin);
            }
        }
        glEnd();
    }

    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_LINE_STRIP);
    {
        for(double x=xBegin; x<xEnd; x+=dx){
            auto y = func(x);
            glVertex2d(x, scale*y);
        }
    }
    glEnd();
}

void RtoR::FunctionRenderer::renderFunction(const R2toR::Function &func, Real xMin, Real yMin, Real L, PosInt nLines, PosInt linesRes, Real scale) {
    const Real xMax = xMin+L;
    const Real yMax = yMin+L;

    const Real dx = (xMax-xMin) / nLines,
            dy = (yMax-yMin) / linesRes;

    glPointSize(2);
    glLineWidth(1.5);

    {
        for (Real x = xMin; x <= xMax; x += dx) {
            glBegin(GL_LINE_STRIP);
            for (Real y = yMin; y < yMax; y += dy) {
                const Real val = func({x,y});
                //const Real colorVal = .5 + .5*sin(atan(func.diff(0, {x, y})));
                //glColor3d(colorVal, colorVal, colorVal);
                glVertex3d(x, y, scale*val);
            }
            glEnd();
        }
    }
    {
        for (Real y = yMin; y <= yMax; y += dx) {
            glBegin(GL_LINE_STRIP);
            for (Real x = xMin; x < xMax; x += dy) {
                const Real val = func({x,y});
                //const Real colorVal = .5 + .5*sin(atan(func.diff(0, {x, y})));
                //glColor3d(colorVal, colorVal, colorVal);
                glVertex3d(x, y, scale*val);
            }
            glEnd();
        }
    }
}

void RtoR::FunctionRenderer::renderSection(const R2toR::Function &func, const RtoR2::StraightLine &section,
                                           Styles::PlotStyle style, PosInt resolution, Real scale) {
    const auto ds = section.getDeltaS() / double(resolution);
    //const auto ds = 1 / double(resolution);
    const auto sMin = section.getSMin(), sMax = section.getSMax();
    //const Real sMin = 0, sMax = 1;

    glLineWidth(style.lineWidth);

    if(style.filled)
    {
        auto c = style.fillColor;

        glColor4f(c.r, c.g, c.b, c.a*.5);
        glBegin(GL_QUADS);
        {
            for(double s=sMin; s<=sMax; s+=ds){
                const double sLeft = s;
                const double sRight = s+ds;

                auto pt1 = section(sLeft),
                     pt2 = section(sRight);

                const double yMin = 0,
                             yMax1 = func(pt1),
                             yMax2 = func(pt2);

                glVertex2d(sLeft, scale*yMin);
                glVertex2d(sLeft, scale*yMax1);
                glVertex2d(sRight, scale*yMax2);
                glVertex2d(sRight, scale*yMin);
            }
        }
        glEnd();
    }

    auto c = style.lineColor;
    glColor4f(c.r, c.g, c.b, c.a);

    if(style.trace != Styles::Solid){
        glDisable(GL_LINE_SMOOTH);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(style.stippleFactor, style.stipplePattern);
    } else glEnable(GL_LINE_SMOOTH);

    glBegin(GL_LINE_STRIP);
    {
        for(double s=sMin; s<=sMax; s+=ds)
            glVertex2d(s, scale*func(section(s)));
    }
    glEnd();

    // ****************** TESTE *********************
    //glBegin(GL_LINES);
    //{
    //    const Real H= 20;
    //    glVertex2d(0, -H);
    //    glVertex2d(0,  H);
//
    //    glVertex2d(   1, -H);
    //    glVertex2d(   1,  H);
    //}
    //glEnd();
}

void RtoR::FunctionRenderer::renderHorizontalSection(const R2toR::Function &func, Styles::Color c, bool filled, Real xMin, Real xMax,
                                                     PosInt resolution) {
    const double dx = (xMax-xMin) / double(resolution);
    const double xBegin = xMin;
    const double xEnd = xMax;
    if(filled)
    {
        glColor4f(c.r, c.g, c.b, c.a/3.0);
        glBegin(GL_QUADS);
        {
            for(double x=xBegin; x<xEnd; x+=dx){
                const double xmin = x;
                const double xmax = x+dx;

                const double ymin = 0,
                        ymax1 = func({xmin, 0.0}),
                        ymax2 = func({xmax, 0.0});

                glVertex2d(xmin, ymin);
                glVertex2d(xmin, ymax1);
                glVertex2d(xmax, ymax2);
                glVertex2d(xmax, ymin);
            }
        }
        glEnd();
    }

    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_LINE_STRIP);
    {
        for(double x=xBegin; x<xEnd; x+=dx){
            glVertex2d(x, func({x,0.}));
        }
    }
    glEnd();
}

void RtoR::FunctionRenderer::renderVerticalSection(const R2toR::Function &func, Styles::Color c, bool filled, Real yMin, Real yMax,
                                                   PosInt resolution) {
    const double dy = (yMax - yMin) / double(resolution);
    const double yBegin = yMin;
    const double yEnd = yMax;
    if(filled)
    {
        glColor4f(c.r, c.g, c.b, c.a/3.0);
        glBegin(GL_QUADS);
        {
            for(double y=yBegin; y < yEnd; y+=dy){
                const double xmin = y;
                const double xmax = y + dy;

                const double ymin = 0,
                        ymax1 = func({0., xmin}),
                        ymax2 = func({0., xmax});

                glVertex2d(xmin, ymin);
                glVertex2d(xmin, ymax1);
                glVertex2d(xmax, ymax2);
                glVertex2d(xmax, ymin);
            }
        }
        glEnd();
    }

    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_LINE_STRIP);
    {
        for(double y=yBegin; y < yEnd; y+=dy)
            glVertex2d(y, func({0.,y}));

    }
    glEnd();
}


