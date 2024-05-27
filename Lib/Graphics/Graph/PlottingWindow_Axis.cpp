//
// Created by joao on 29/08/23.
//

#include "PlottingWindow.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

#include "PlotThemeManager.h"


namespace Slab {

    Str Graphics::PlottingWindow::getXHairLabel(const Point2D &coords) const {
        fix digits = 5;
        auto &hUnit = axisArtist.getHorizontalUnit();
        auto &vUnit = axisArtist.getVerticalUnit();

        Str hAxisName = axisArtist.getHorizontalAxisLabel();
        Str vAxisName = axisArtist.getVerticalAxisLabel();
        auto baseLabel = Str("(") + hAxisName + ", " + vAxisName + ") = (" + hUnit(coords.x, digits) + ", " +
                         vUnit(coords.y, digits) + ")";

        for (IN contie: content) {
            IN artie = contie.second;

            if (artie->getRegion().doesHit(coords.x, coords.y)) {
                auto artLabel = artie->getXHairInfo(coords);

                if (artLabel == "") continue;

                baseLabel += " --> " + artLabel;
            }
        }

        return baseLabel;
    }

    void Graphics::PlottingWindow::reviewGraphRanges() {
        RectR newRegion = region;

        for (auto &a: content) {
            auto &artist = a.second;

            if(artist->affectsGraphRanges()) {
                newRegion = artist->getRegion();
                break;
            }
        }

        for (auto &a: content) {
            auto &artist = a.second;

            if (!artist->affectsGraphRanges()) continue;

            auto aRegion = artist->getRegion();

            if (aRegion.xMax > newRegion.xMax) newRegion.xMax = aRegion.xMax;
            if (aRegion.xMin < newRegion.xMin) newRegion.xMin = aRegion.xMin;
            if (aRegion.yMax > newRegion.yMax) newRegion.yMax = aRegion.yMax;
            if (aRegion.yMin < newRegion.yMin) newRegion.yMin = aRegion.yMin;
        }

        if(newRegion != region) {
            region = newRegion;

            auto currStyle = PlotThemeManager::GetCurrent();
            auto pix = PixelSizeInSpace(region, getViewport());
            auto Δx = (Real)currStyle->hAxisPaddingInPixels * pix.x;
            auto Δy = (Real)currStyle->hAxisPaddingInPixels * pix.y;

            region.xMin -= 2*Δx;
            region.xMax += 2*Δx;
            region.yMin -= 2*Δy;
            region.yMax += 2*Δy;
        }
    }

    void Graphics::PlottingWindow::nameLabelDraw(const Graphics::PlotStyle &style, const Str &label) {
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (0)");

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

        if (style.filled) {
            auto color = style.fillColor;
            glColor4f(color.r, color.g, color.b, .5 * color.a);

            glRectd(xMin_label, yMin_label, xMax_label, yMax_label);
        }

        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (1)");

        {
            auto color = style.lineColor;

            glColor4f(color.r, color.g, color.b, color.a);
            glLineWidth(style.thickness);

            if (style.primitive != Graphics::Solid) {
                glDisable(GL_LINE_SMOOTH);
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(style.stippleFactor, style.stipplePattern);
            } else {
                glEnable(GL_LINE_SMOOTH);
                glDisable(GL_LINE_STIPPLE);
            }

            if (style.filled) {
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

        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (2)");

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);
        glLineWidth(1.5);

        auto currStyle = PlotThemeManager::GetCurrent();

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


}