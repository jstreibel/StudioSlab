//
// Created by joao on 31/08/23.
//

#include "LabelsArtist.h"

#include "3rdParty/ImGui.h"

#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/Utils.h"
#include "Graphics/Plot2D/PlotStyle.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

namespace Slab::Graphics {
    void LabelsArtist::setTotalItems(CountType tot) {
        currItem=0;

        switch (tot) {
            case 1:
            case 2:
            case 3:
                cols = Common::min(1, max_cols);
                break;
            case 4:
            case 5:
            case 6:
                cols=Common::min(2, max_cols);
                break;
            case 7:
            case 8:
            case 9:
                cols=Common::min(3, max_cols);
                break;
            default:
                cols=Common::min(4, max_cols);
        }
    }

    CountType LabelsArtist::row() const { return currItem / cols; }
    CountType LabelsArtist::col() const { return currItem % cols; }

    LabelsArtist &LabelsArtist::operator++() { currItem++; return *this; }

    bool LabelsArtist::draw(const Plot2DWindow &window) {
        setTotalItems(labelsRequired.size());

        auto viewport = window.getViewport();

        for(auto &[label, style] : labelsRequired)
            draw_label(*style, label, viewport);

        labelsRequired.clear();

        return true;
    }

    void LabelsArtist::draw_label(PlotStyle &style, const Str& label, const RectI &viewport) {
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (0)");

        OpenGL::Shader::remove();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 1, 0, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        fix col = this->col();
        fix row = this->row();
        ++ (*this);

        auto colWidth = 1.f/float(2*cols-1);
        auto xMin_label = x_offset + (colWidth + xGap + dx) * float(col),
             xMax_label = xMin_label + dx,
             yMin_label = y_offset + ((-yGap) + (-dy)) * float(row),
             yMax_label = yMin_label + (-dy);

        if (style.filled) {
            auto color = style.fillColor;
            glColor4f(color.r, color.g, color.b, .5f * color.a);

            glRectd(xMin_label, yMin_label, xMax_label, yMax_label);
        }

        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (1)");

        {
            auto color = style.lineColor;

            glColor4f(color.r, color.g, color.b, color.a);
            glLineWidth(style.thickness);

            if (style.getPrimitive() != Graphics::Solid) {
                glDisable(GL_LINE_SMOOTH);
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(style.getStippleFactor(), style.getStipplePattern());
            } else {
                glEnable(GL_LINE_SMOOTH);
                glDisable(GL_LINE_STIPPLE);
            }

            if (style.filled) {
                glBegin(GL_LINE_LOOP);
                glVertex2d(xMin_label, yMin_label);
                glVertex2d(xMax_label, yMin_label);
                glVertex2d(xMax_label, yMax_label);
                glVertex2d(xMin_label, yMax_label);
                glEnd();
            } else {
                glBegin(GL_LINES);
                glVertex2d(xMin_label, .5 * (yMin_label + yMax_label));
                glVertex2d(xMax_label, .5 * (yMin_label + yMax_label));
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
        loc = FromSpaceToViewportCoord(loc, {0, 1, 0, 1}, viewport);
        writer->write(label, {loc.x, loc.y}, c);

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }

    void
    LabelsArtist::add(const Str& label, const Pointer<PlotStyle>& style)
    { labelsRequired.emplace_back(label, style); }

    bool LabelsArtist::hasGUI() {
        return true;
    }

    void LabelsArtist::drawGUI() {
        ImGui::SliderInt("Max cols", &max_cols, 1, 4);

        ImGui::SliderFloat("Δx", &dx, .01f, .20f);
        ImGui::SliderFloat("Δy", &dy, .01f, .20f);

        ImGui::SliderFloat("x offset (%)", &x_offset, .0f, 1.f);
        ImGui::SliderFloat("y offset (%)", &y_offset, .0f, 1.f);

        ImGui::SliderFloat("gap x", &xGap, .01f, .20f);
        ImGui::SliderFloat("gap y", &yGap, .01f, .20f);
    }


}