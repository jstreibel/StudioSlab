//
// Created by joao on 20/05/24.
//
#include "PointSetRenderer.h"
#include "Graphics/OpenGL/Shader.h"


namespace Slab::Graphics::OpenGL {

    bool Legacy::RenderPointSet(const Math::PointSet_constptr& pSet, PlotStyle style) noexcept {
        if(pSet== nullptr) return true;

        IN pts = pSet->getPoints();

        if(pts.empty()) return true;

        Shader::remove();

        if (style.filled && !(style.getPrimitive() == Point || style.getPrimitive() == LinePrimitive::Lines)) {
            const auto color = style.fillColor;

            glColor4f(color.r, color.g, color.b, color.a);
            glBegin(GL_QUADS);
            {
                auto iMax = (long) pts.size() - 1;
                for (auto i = 0; i < iMax; ++i) {
                    auto pLeft = pts[i];
                    auto pRite = pts[i + 1];

                    const DevFloat xmin = pLeft.x;
                    const DevFloat xmax = pRite.x;

                    const DevFloat ymin = 0,
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

            if (style.getPrimitive() != Solid
                && style.getPrimitive() != VerticalLines
                && style.getPrimitive() != LinePrimitive::Lines) {
                glDisable(GL_LINE_SMOOTH);
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(style.getStippleFactor(), style.getStipplePattern());
            } else glEnable(GL_LINE_SMOOTH);

            auto primitive = GL_LINE_STRIP;
            if (style.getPrimitive() == Point || style.getPrimitive() == VerticalLines) {
                fix ptSizeFactor = style.getPrimitive() == VerticalLines ? 5.0 : 1.0;

                primitive = GL_POINTS;
                glEnable(GL_POINT_SMOOTH);
                glPointSize(style.thickness * ptSizeFactor);

                glEnable(GL_LINE_SMOOTH);
                glLineWidth(style.thickness);
            } else if (style.getPrimitive() == LinePrimitive::Lines) {
                primitive = GL_LINES;
            }

            glBegin(primitive);
            {
                for (auto p: pts)
                    glVertex2d(p.x, p.y);

            }
            glEnd();

            if (style.getPrimitive() == VerticalLines) {
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

        return !OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__));
    }


}