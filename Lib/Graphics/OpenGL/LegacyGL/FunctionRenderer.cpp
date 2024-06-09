//

#include "FunctionRenderer.h"


//
// Created by joao on 27/09/2019.


namespace Slab {

    void
    Graphics::FunctionRenderer::renderFunction(const RtoR::NumericFunction &func, Color c, bool filled, Real scale) {
        const Real xMin = func.xMin,
                xMax = func.xMax;
        const int N = func.N;

        renderFunction(func, c, filled, xMin, xMax, N, scale);
    }

    void
    Graphics::FunctionRenderer::renderFunction(const RtoR::Function &func, Color c, bool filled, Real xMin, Real xMax,
                                               UInt resolution, Real scale) {
        const Real dx = (xMax - xMin) / Real(resolution);
        const Real xBegin = xMin;
        const Real xEnd = xMax;

        if (dx == .0) return;

        Graphics::OpenGL::Shader::remove();

        if (filled) {
            glColor4f(c.r, c.g, c.b, c.a / 3.0);
            glBegin(GL_QUADS);
            {
                for (Real x = xBegin; x < xEnd; x += dx) {
                    const Real xmin = x;
                    const Real xmax = x + dx;

                    const Real ymin = 0,
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
            for (Real x = xBegin; x < xEnd; x += dx) {
                auto y = func(x);
                glVertex2d(x, scale * y);
            }
        }
        glEnd();
    }

    void
    Graphics::FunctionRenderer::renderFunction(const R2toR::Function &func, Real xMin, Real yMin, Real L, UInt nLines,
                                               UInt linesRes, Real scale) {
        const Real xMax = xMin + L;
        const Real yMax = yMin + L;

        const Real dx = (xMax - xMin) / nLines,
                dy = (yMax - yMin) / linesRes;

        Graphics::OpenGL::Shader::remove();

        glPointSize(2);
        glLineWidth(1.5);

        {
            for (Real x = xMin; x <= xMax; x += dx) {
                glBegin(GL_LINE_STRIP);
                for (Real y = yMin; y < yMax; y += dy) {
                    const Real val = func({x, y});
                    //const Real colorVal = .5 + .5*sin(atan(func.diff(0, {x, y})));
                    //glColor3d(colorVal, colorVal, colorVal);
                    glVertex3d(x, y, scale * val);
                }
                glEnd();
            }
        }
        {
            for (Real y = yMin; y <= yMax; y += dx) {
                glBegin(GL_LINE_STRIP);
                for (Real x = xMin; x < xMax; x += dy) {
                    const Real val = func({x, y});
                    //const Real colorVal = .5 + .5*sin(atan(func.diff(0, {x, y})));
                    //glColor3d(colorVal, colorVal, colorVal);
                    glVertex3d(x, y, scale * val);
                }
                glEnd();
            }
        }
    }

    void Graphics::FunctionRenderer::renderSection(const R2toR::Function &func, const RtoR2::ParametricCurve &section,
                                                   PlotStyle style, UInt resolution, Real scale) {
        const auto ds = section.getΔs() / Real(resolution);
        const auto sMin = section.get_sMin(), sMax = section.get_sMax();

        Graphics::OpenGL::Shader::remove();

        glLineWidth(style.thickness);

        if (style.filled) {
            auto c = style.fillColor;

            glColor4f(c.r, c.g, c.b, c.a * .5);
            glBegin(GL_QUADS);
            {
                for (Real s = sMin; s <= sMax; s += ds) {
                    const Real sLeft = s;
                    const Real sRight = s + ds;

                    auto pt1 = section(sLeft),
                            pt2 = section(sRight);

                    const Real yMin = 0,
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

        if (style.primitive != Solid) {
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else glEnable(GL_LINE_SMOOTH);

        glBegin(GL_LINE_STRIP);
        {
            for (Real s = sMin; s <= sMax; s += ds)
                glVertex2d(s, scale * func(section(s)));
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

    void
    Graphics::FunctionRenderer::renderHorizontalSection(const R2toR::Function &func, Color c, bool filled, Real xMin,
                                                        Real xMax,
                                                        UInt resolution) {
        const Real dx = (xMax - xMin) / Real(resolution);
        const Real xBegin = xMin;
        const Real xEnd = xMax;
        if (filled) {
            Graphics::OpenGL::Shader::remove();

            glColor4f(c.r, c.g, c.b, c.a / 3.0);
            glBegin(GL_QUADS);
            {
                for (Real x = xBegin; x < xEnd; x += dx) {
                    const Real xmin = x;
                    const Real xmax = x + dx;

                    const Real ymin = 0,
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
            for (Real x = xBegin; x < xEnd; x += dx) {
                glVertex2d(x, func({x, 0.}));
            }
        }
        glEnd();
    }

    void Graphics::FunctionRenderer::renderVerticalSection(const R2toR::Function &func, Color c, bool filled, Real yMin,
                                                           Real yMax,
                                                           UInt resolution) {
        Graphics::OpenGL::Shader::remove();

        const Real dy = (yMax - yMin) / Real(resolution);
        const Real yBegin = yMin;
        const Real yEnd = yMax;

        if (filled) {

            glColor4f(c.r, c.g, c.b, c.a / 3.0);
            glBegin(GL_QUADS);
            {
                for (Real y = yBegin; y < yEnd; y += dy) {
                    const Real xmin = y;
                    const Real xmax = y + dy;

                    const Real ymin = 0,
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
            for (Real y = yBegin; y < yEnd; y += dy)
                glVertex2d(y, func({0., y}));

        }
        glEnd();
    }


}