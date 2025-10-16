//
// Created by joao on 20/05/24.
//
#include "PointSetRenderer.h"
#include "Graphics/OpenGL/Shader.h"


namespace Slab::Graphics::OpenGL {

    void RenderPlotOrPointset(const Point2DVec& pts, PlotStyle style) noexcept {

        if (style.filled && !(style.getPrimitive() == Points || style.getPrimitive() == Lines)) {
            const auto color = style.fillColor;

            glColor4f(color.r, color.g, color.b, color.a);
            glBegin(GL_QUADS);
            {
                const auto iMax = static_cast<long>(pts.size()) - 1;
                for (auto i = 0; i < iMax; ++i) {
                    const auto pLeft = pts[i], pRite = pts[i + 1];

                    const DevFloat
                    xMin = pLeft.x,
                    xMax = pRite.x,
                    yMin = 0,
                    yMax1 = pLeft.y,
                    yMax2 = pRite.y;

                    glVertex2d(xMin, yMin);
                    glVertex2d(xMin, yMax1);
                    glVertex2d(xMax, yMax2);
                    glVertex2d(xMax, yMin);
                }
            }
            glEnd();
        }

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));

        {
            glLineWidth(style.thickness);

            auto color = style.lineColor;
            glColor4f(color.r, color.g, color.b, color.a);

            if (style.getPrimitive() != PlottingSolid
                && style.getPrimitive() != PlottingVerticalLinesWithCircles
                && style.getPrimitive() != LinePrimitive::Lines) {
                glDisable(GL_LINE_SMOOTH);
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(style.getStippleFactor(), style.getStipplePattern());
            } else glEnable(GL_LINE_SMOOTH);

            auto primitive = GL_LINE_STRIP;
            if (style.getPrimitive() == Points || style.getPrimitive() == PlottingVerticalLinesWithCircles) {
                fix ptSizeFactor = style.getPrimitive() == PlottingVerticalLinesWithCircles ? 5.0 : 1.0;

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

            if (style.getPrimitive() == PlottingVerticalLinesWithCircles) {
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
    }

    void RenderOtherPrimitives(const Point2DVec& pts, PlotStyle style) noexcept {

        const auto color = style.fillColor;
        glColor4f(color.r, color.g, color.b, color.a);

        GLenum Primitive;
        switch (style.getPrimitive()) {
            case LineStrip:
                Primitive = GL_LINE_STRIP;
                break;
            case LineLoop:
                Primitive = GL_LINE_LOOP;
                break;
            case Triangles:
                Primitive = GL_TRIANGLES;
                break;
            case Quads:
                Primitive = GL_QUADS;
                break;
            default:
                return;
        }

        glBegin(Primitive);
        for (const auto p: pts) glVertex2d(p.x, p.y);
        glEnd();
    }

    bool Legacy::RenderPointSet(const PointSet_constptr& pSet, const PlotStyle& style) noexcept {
        if(pSet== nullptr) return true;

        IN pts = pSet->getPoints();

        if(pts.empty()) return true;

        switch (style.getPrimitive()) {
        case PlottingSolid:
        case PlottingDotted:
        case PlottingDashed:
        case PlottingDotDashed:
        case PlottingVerticalLinesWithCircles:
        case Lines:
        case Points:
            RenderPlotOrPointset(pts, style);
            break;

        case LineStrip:
        case LineLoop:
        case Triangles:
        case Quads:
            RenderOtherPrimitives(pts, style);
            break;
        case __COUNT__:
            break;
        }

        return !CheckGLErrors(Str(__PRETTY_FUNCTION__));
    }


}