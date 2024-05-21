//
// Created by joao on 20/05/24.
//
#include "PointSetRenderer.h"

void Graphics::OpenGL::Legacy::RenderPointSet(Math::PointSet_constptr pSet, PlotStyle style) noexcept {
    auto pts = pSet->getPoints();

    OpenGL::Shader::remove();

    if (style.filled && !(style.primitive == Point || style.primitive == LinePrimitive::Lines)) {
        const auto color = style.fillColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glBegin(GL_QUADS);
        {
            auto iMax = (long) pts.size() - 1;
            for (auto i = 0; i < iMax; ++i) {
                auto pLeft = pts[i];
                auto pRite = pts[i + 1];

                const Real xmin = pLeft.x;
                const Real xmax = pRite.x;

                const Real ymin = 0,
                        ymax1 = pLeft.y,
                        ymax2 = pRite.y;

                glVertex2d(xmin, ymin);
                glVertex2d(xmin, ymax1);
                glVertex2d(xmax, ymax2);
                glVertex2d(xmax, ymin);
            }
        }
        glEnd();
    }

    {
        glLineWidth(style.thickness);

        auto color = style.lineColor;
        glColor4f(color.r, color.g, color.b, color.a);

        if (style.primitive != Solid
            && style.primitive != VerticalLines
            && style.primitive != LinePrimitive::Lines) {
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else glEnable(GL_LINE_SMOOTH);

        auto primitive = GL_LINE_STRIP;
        if (style.primitive == Point || style.primitive == VerticalLines) {
            fix ptSizeFactor = style.primitive == VerticalLines ? 5.0 : 1.0;

            primitive = GL_POINTS;
            glEnable(GL_POINT_SMOOTH);
            glPointSize(style.thickness * ptSizeFactor);

            glEnable(GL_LINE_SMOOTH);
            glLineWidth(style.thickness);
        } else if (style.primitive == LinePrimitive::Lines) {
            primitive = GL_LINES;
        }

        glBegin(primitive);
        {
            for (auto p: pts)
                glVertex2d(p.x, p.y);

        }
        glEnd();

        if (style.primitive == VerticalLines) {
            glBegin(GL_LINES);
            {
                for (auto p: pts) {
                    glVertex2d(p.x, 0.0);
                    glVertex2d(p.x, p.y);
                }
            }
            glEnd();
        }

    }

    OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__));
}

