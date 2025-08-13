//

#include "FunctionRenderer.h"
#include "Graphics/OpenGL/Shader.h"



//
// Created by joao on 27/09/2019.


namespace Slab::Graphics::OpenGL::Legacy {

    void
    FunctionRenderere::RenderFunction(const RtoR::NumericFunction &func, FColor c, bool filled, DevFloat xMin, DevFloat xMax, DevFloat scale) {
        const int N = func.N;

        RenderFunction(func, c, filled, xMin, xMax, N, scale);
    }

    void
    FunctionRenderere::RenderFunction(const RtoR::Function &func, FColor c, bool filled, DevFloat xMin, DevFloat xMax,
                                               UInt resolution, DevFloat scale) {
        const DevFloat dx = (xMax - xMin) / DevFloat(resolution);
        const DevFloat xBegin = xMin;
        const DevFloat xEnd = xMax;

        if (dx == .0) return;

        if (filled) {
            glColor4f(c.r, c.g, c.b, c.a / 3.0);
            glBegin(GL_QUADS);
            {
                for (DevFloat x = xBegin; x < xEnd; x += dx) {
                    const DevFloat xmin = x;
                    const DevFloat xmax = x + dx;

                    const DevFloat ymin = 0,
                            ymax1 = scale * func(xmin),
                            ymax2 = scale * func(xmax);

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
            for (DevFloat x = xBegin; x < xEnd; x += dx) {
                auto y = func(x);
                glVertex2d(x, scale * y);
            }
        }
        glEnd();
    }

    void
    FunctionRenderere::RenderFunction(const R2toR::Function &func, DevFloat xMin, DevFloat yMin, DevFloat L, UInt nLines,
                                               UInt linesRes, DevFloat scale) {
        const DevFloat xMax = xMin + L;
        const DevFloat yMax = yMin + L;

        const DevFloat dx = (xMax - xMin) / nLines,
                dy = (yMax - yMin) / linesRes;

        glPointSize(2);
        glLineWidth(1.5);

        {
            for (DevFloat x = xMin; x <= xMax; x += dx) {
                glBegin(GL_LINE_STRIP);
                for (DevFloat y = yMin; y < yMax; y += dy) {
                    const DevFloat val = func({x, y});
                    //const DevFloat colorVal = .5 + .5*sin(atan(func.diff(0, {x, y})));
                    //glColor3d(colorVal, colorVal, colorVal);
                    glVertex3d(x, y, scale * val);
                }
                glEnd();
            }
        }
        {
            for (DevFloat y = yMin; y <= yMax; y += dx) {
                glBegin(GL_LINE_STRIP);
                for (DevFloat x = xMin; x < xMax; x += dy) {
                    const DevFloat val = func({x, y});
                    //const DevFloat colorVal = .5 + .5*sin(atan(func.diff(0, {x, y})));
                    //glColor3d(colorVal, colorVal, colorVal);
                    glVertex3d(x, y, scale * val);
                }
                glEnd();
            }
        }
    }

    void FunctionRenderere::RenderSection(const R2toR::Function &func, const RtoR2::ParametricCurve &section,
                                                   PlotStyle style, UInt resolution, DevFloat scale) {
        const auto ds = section.getΔs() / DevFloat(resolution);
        const auto sMin = section.get_sMin(), sMax = section.get_sMax();

        glLineWidth(style.thickness);

        if (style.filled) {
            auto c = style.fillColor;

            glColor4f(c.r, c.g, c.b, c.a * .5);
            glBegin(GL_QUADS);
            {
                for (DevFloat s = sMin; s <= sMax; s += ds) {
                    const DevFloat sLeft = s;
                    const DevFloat sRight = s + ds;

                    auto pt1 = section(sLeft),
                            pt2 = section(sRight);

                    const DevFloat yMin = 0,
                            yMax1 = func(pt1),
                            yMax2 = func(pt2);

                    glVertex2d(sLeft, scale * yMin);
                    glVertex2d(sLeft, scale * yMax1);
                    glVertex2d(sRight, scale * yMax2);
                    glVertex2d(sRight, scale * yMin);
                }
            }
            glEnd();
        }

        auto c = style.lineColor;
        glColor4f(c.r, c.g, c.b, c.a);

        if (style.getPrimitive() != Solid) {
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.getStippleFactor(), style.getStipplePattern());
        } else glEnable(GL_LINE_SMOOTH);

        GLenum mode = GL_LINE_STRIP;
        if(style.getPrimitive()==Point || style.getPrimitive()==VerticalLines) {
            mode = GL_POINTS;
            glPointSize(10*style.thickness);
            glLineWidth(style.thickness);
            glEnable(GL_POINT_SMOOTH);
            glDisable(GL_LINE_STIPPLE);
        }

        if(style.getPrimitive()==VerticalLines) {
            glBegin(GL_LINES);
            {
                for (DevFloat s = sMin; s <= sMax; s += ds) {
                    fix y0 = 0.0;
                    fix y1 = scale * func(section(s));
                    glVertex2d(s, y0);
                    glVertex2d(s, y1);
                }
            }
            glEnd();
        }

        glBegin(mode);
        {
            for (DevFloat s = sMin; s <= sMax; s += ds)
                glVertex2d(s, scale * func(section(s)));
        }
        glEnd();

    }

    void
    FunctionRenderere::RenderHorizontalSection(const R2toR::Function &func, FColor c, bool filled, DevFloat xMin,
                                                        DevFloat xMax,
                                                        UInt resolution) {
        const DevFloat dx = (xMax - xMin) / DevFloat(resolution);
        const DevFloat xBegin = xMin;
        const DevFloat xEnd = xMax;
        if (filled) {

            glColor4f(c.r, c.g, c.b, c.a / 3.0);
            glBegin(GL_QUADS);
            {
                for (DevFloat x = xBegin; x < xEnd; x += dx) {
                    const DevFloat xmin = x;
                    const DevFloat xmax = x + dx;

                    const DevFloat ymin = 0,
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
            for (DevFloat x = xBegin; x < xEnd; x += dx) {
                glVertex2d(x, func({x, 0.}));
            }
        }
        glEnd();
    }

    void FunctionRenderere::RenderVerticalSection(const R2toR::Function &func, FColor c, bool filled, DevFloat yMin,
                                                           DevFloat yMax,
                                                           UInt resolution) {

        const DevFloat dy = (yMax - yMin) / DevFloat(resolution);
        const DevFloat yBegin = yMin;
        const DevFloat yEnd = yMax;

        if (filled) {

            glColor4f(c.r, c.g, c.b, c.a / 3.0);
            glBegin(GL_QUADS);
            {
                for (DevFloat y = yBegin; y < yEnd; y += dy) {
                    const DevFloat xmin = y;
                    const DevFloat xmax = y + dy;

                    const DevFloat ymin = 0,
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
            for (DevFloat y = yBegin; y < yEnd; y += dy)
                glVertex2d(y, func({0., y}));

        }
        glEnd();
    }


}