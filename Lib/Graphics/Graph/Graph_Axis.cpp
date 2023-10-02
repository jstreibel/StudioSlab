//
// Created by joao on 29/08/23.
//

#include "Graph.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

#include "imgui.h"
#include "StylesManager.h"

Str Graphics::Graph2D::getXHairLabel(const Point2D &coords) const {
    fix digits = 5;
    return Str("(")+ baseHorizontalUnit(coords.x, digits) + ", " + baseVerticalUnit(coords.y, digits) + ")";
}

void Graphics::Graph2D::reviewGraphRanges() {
    if(!mPointSets.empty())
    {
        auto referencePointSet = mPointSets.begin()->data;

        region.xMax = referencePointSet->getMax().x;
        region.xMin = referencePointSet->getMin().x;
        region.yMax = referencePointSet->getMax().y;
        region.yMin = referencePointSet->getMin().y;

        for (auto &set: mPointSets) {
            if(!set.affectsGraphRanges) continue;

            auto max = set.data->getMax();
            auto min = set.data->getMin();

            if (max.x > region.xMax) region.xMax = max.x;
            if (min.x < region.xMin) region.xMin = min.x;
            if (max.y > region.yMax) region.yMax = max.y;
            if (min.y < region.yMin) region.yMin = min.y;
        }

        if(true)
        {
            const auto dMin = .1;

            auto dx = dMin * (region.xMax - region.xMin);
            auto dy = dMin * (region.yMax - region.yMin);

            if(dx == 0) dx = dMin;
            if(dy == 0) dy = dMin;

            region.xMax += .5*dx;
            region.xMin -= dx;
            region.yMax += dy;
            region.yMin -= dy;
        }
    }
}

void Graphics::Graph2D::nameLabelDraw(const Styles::PlotStyle &style, const Str& label) {
    OpenGL::Shader::remove();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    fix col = labelingHelper.col();
    fix row = labelingHelper.row();
    ++labelingHelper;

    auto dx = .080,
            dy = -.060;
    auto xGap = 0.015,
            yGap = -.025;
    auto colWidth = 0.5;
    auto xMin_label = .100 + (colWidth + xGap + dx) * float(col),
            xMax_label = xMin_label + dx,
            yMin_label = .975 + (yGap + dy) * float(row),
            yMax_label = yMin_label + dy;

    if(style.filled) {
        auto color = style.fillColor;
        glColor4f(color.r, color.g, color.b, .5 * color.a);

        glRectd(xMin_label, yMin_label, xMax_label, yMax_label);
    }

    {
        auto color = style.lineColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glLineWidth(style.thickness);

        if (style.primitive != Styles::SolidLine) {
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else {
            glEnable(GL_LINE_SMOOTH);
            glDisable(GL_LINE_STIPPLE);
        }

        if(style.filled) {
            glBegin(GL_LINE_LOOP);
            glVertex2f(xMin_label, yMin_label);
            glVertex2f(xMax_label, yMin_label);
            glVertex2f(xMax_label, yMax_label);
            glVertex2f(xMin_label, yMax_label);
            glEnd();
        } else {
            glBegin(GL_LINES);
            glVertex2f(xMin_label, .5 * (yMin_label + yMax_label));
            glVertex2f(xMax_label, .5 * (yMin_label + yMax_label));
            glEnd();
        }
    }

    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.5);

    auto currStyle = Math::StylesManager::GetCurrent();

    auto c = currStyle->graphNumbersColor;
    Point2D loc = {xMax_label + xGap, .5 * (yMax_label + yMin_label)};

    auto writer = currStyle->labelsWriter;
    loc = FromSpaceToViewportCoord(loc, {0, 1, 0, 1}, getViewport());
    writer->write(label, {loc.x, loc.y}, c);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}