//
// Created by joao on 1/10/23.
//

#include "3rdParty/ImGui.h"

#include "Utils/EncodingUtils.h"

#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

#include "AxisArtist.h"

#include <utility>


#define hPixelsToSpaceScale (region.width() / vp.width())
#define vPixelsToSpaceScale (region.height() / vp.height())

#define USE_ELEGANT_SCIENTIFIC false

#define setupBuffer(buffer) \
    if     (numRegion> +2)  (buffer) << std::setprecision(0); \
    else if(numRegion> +1)  (buffer) << std::setprecision(1); \
    else                    (buffer) << std::setprecision(2); \
                                                              \
    if     (numRegion< -1){                                   \
        (buffer) << std::scientific;                          \
        if(iMin<0 && iMax>0) (buffer) << std::setprecision(0);\
    }                                                         \
    else                    (buffer) << std::fixed;           \
                                                              \
    if     (mark < 0)       (buffer) << "";                   \
    else (buffer) << " ";                                     \
                                                              \

namespace Slab::Graphics {

    AxisArtist::AxisArtist()
    : AxisArtist(Constants::One, Constants::One)
    {    }

    AxisArtist::AxisArtist(const Unit& horizontal, const Unit& vertical)
    : hUnit(horizontal)
    , vUnit(vertical)
    {    }

    bool AxisArtist::draw(const Plot2DWindow &graph) {
        glLineWidth(1.0);

        computeTicks(graph);

        drawXAxis(graph);
        drawYAxis(graph);

        return true;
    }

    void AxisArtist::computeTicks(const Plot2DWindow &graph) {
        const auto& region = graph.getRegion();

        auto currStyle = PlotThemeManager::GetCurrent();

        auto writer = currStyle->ticksWriter;

        {
            const Real Δy = region.height() / vUnit.value();

            const auto theLog = log10(Δy);
            const auto spacing = pow(10., floor(theLog) - 1.);
            const auto theRest = theLog - floor(theLog);
            const auto multiplier = y_spacing_multiplier * floor(pow(10., theRest));

            ySpacing = multiplier * spacing;
        }

        if(!hTicksManual){
            hTicks.clear();

            const auto Δx = region.width() / hUnit.value();

            const auto theLog = log10(Δx);
            const auto spacing = pow(10., floor(theLog) - 1.);
            const auto theRest = theLog - floor(theLog);
            const auto multiplier = x_spacing_multiplier * floor(pow(10., theRest));

            xSpacing = multiplier * spacing;

            fix iMin = int(region.getXMin()/xSpacing);
            fix iMax = int(region.getXMax()/xSpacing);

            for (auto i=iMin; i<=iMax; ++i)
            {
                fix mark = i*xSpacing;
                auto label = hUnit(mark, 2);

                hTicks.push_back({mark, label});
            }
        }
    }

    void AxisArtist::drawXAxis(const Plot2DWindow &graph) const {
        auto vp = graph.getViewport();
        const auto& region = graph.getRegion();

        auto currStyle = PlotThemeManager::GetCurrent();

        auto writer = currStyle->ticksWriter;
        fix fontHeight = writer->getFontHeightInPixels();

        fix vTickHeightInSpace = 2.0*(Real)currStyle->vTickHeightinPixels * vPixelsToSpaceScale;
        fix vGraphPaddingInSpace = (Real)currStyle->vAxisPaddingInPixels * vPixelsToSpaceScale;

        fix yLocationOfXAxis = region.getYMin() < -vGraphPaddingInSpace
                                           ? 0
                                           : region.getYMin() + vGraphPaddingInSpace;

        fix yLocationOfLabels = yLocationOfXAxis -1.1 * (vTickHeightInSpace+fontHeight) * vPixelsToSpaceScale;

        auto &gtfColor = currStyle->graphNumbersColor;

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);

        // Write numbers
        for (auto &hTick : hTicks) {
            auto pen = FromSpaceToViewportCoord({hTick.mark, yLocationOfLabels}, region.getRect(), vp);
            writer->write(hTick.label, pen, gtfColor);
        }

        // Draw axes
        {
            OpenGL::Shader::remove();

            auto &ac = currStyle->axisColor;
            auto &tc = currStyle->majorTickColor;
            glLineWidth(currStyle->majorGridLines.thickness);
            glBegin(GL_LINES);
            {
                // Draw x-axis
                glColor4f(ac.r, ac.g, ac.b, ac.a);

                glVertex3d(region.getXMin(), yLocationOfXAxis, 0);
                glVertex3d(region.getXMax(), yLocationOfXAxis, 0);


                // Draw ticks
                glColor4f(tc.r, tc.g, tc.b, tc.a);
                for (auto &hTick : hTicks){
                    Real mark = hTick.mark;
                    glVertex3d(mark, -vTickHeightInSpace, 0);
                    glVertex3d(mark, +vTickHeightInSpace, 0);
                }
            }
            glEnd();
        }

        // Draw axis name
        {
            const auto xMidpoint = region.xCenter();
            const auto yMidpoint = region.getYMin();
            const Point2D loc = {xMidpoint, yMidpoint};
            writer = currStyle->labelsWriter;

            auto pen = FromSpaceToViewportCoord(loc, region.getRect(), vp);
            pen.y += writer->getFontHeightInPixels();

            writer->write(horizontalAxisLabel, pen , currStyle->graphTitleColor);
        }

        Graphics::OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__));
    }

    void AxisArtist::drawYAxis(const Plot2DWindow &graph) const {
        auto vp = graph.getViewport();
        const auto& region = graph.getRegion();
        auto currStyle = PlotThemeManager::GetCurrent();

        auto writer = currStyle->ticksWriter;

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        fix Δy = region.height();
        fix xLocationOfYAxis = region.getXMin() + (Real)currStyle->hAxisPaddingInPixels*hPixelsToSpaceScale;
        fix yOffsetOfLabels = 0.2*writer->getFontHeightInPixels()* vPixelsToSpaceScale;
        fix iMin = int(region.getYMin()/ySpacing);
        fix iMax = int(region.getYMax()/ySpacing);

        StringStream buffer;

        auto &gtf = currStyle->graphNumbersColor;
        glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
        auto numRegion = log10(Δy);

        for(int i=iMin; i<=iMax; ++i){
            auto mark = i*ySpacing;

            auto label = vUnit(mark, 2);

            Point2D loc = {float(xLocationOfYAxis), float(mark)+yOffsetOfLabels};

            Str text;
            if(0) {
                buffer.str("");

                setupBuffer(buffer);

                buffer << mark;

                text = buffer.str();
                if (numRegion < -1 && USE_ELEGANT_SCIENTIFIC) text = elegantScientific(text);
            } else text = vUnit(mark, 2);

            loc = FromSpaceToViewportCoord(loc, region.getRect(), vp);
            writer->write(text, loc, gtf);
        }


        OpenGL::Shader::remove();

        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LINE_SMOOTH);

        glDisable(GL_LINE_STIPPLE);
        glBegin(GL_LINES);
        {
            auto &ac = currStyle->axisColor;

            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(region.getXMin(), 0, 0);
            glVertex3d(region.getXMax(), 0, 0);
        }
        glEnd();


        /* Minor grid lines */
        glEnable(GL_LINE_STIPPLE);
        // glLineStipple(2, 0x2727);
        glLineStipple(currStyle->minorGridLines.getStippleFactor(), currStyle->minorGridLines.getStipplePattern());
        // glLineStipple(2, 0x1111);
        glLineWidth(currStyle->minorGridLines.thickness);

        glBegin(GL_LINES);
        {
            auto &ac = currStyle->axisColor;
            auto &tc = currStyle->minorGridLines.lineColor;

            glColor4f(tc.r, tc.g, tc.b, tc.a);

            for(auto i = iMin; i<=iMax; ++i) {
                if(i==0) continue;

                Real mark = i*ySpacing;
                glVertex3d(region.getXMin(), mark, 0);
                glVertex3d(region.getXMax(), mark, 0);
            }

            // glColor4f(ac.r, ac.g, ac.b, ac.a);

            // glVertex3d(region.getXMin(), 0, 0);
            // glVertex3d(region.getXMax(), 0, 0);
        }
        glEnd();
        glPopAttrib();

        // Draw axis name
        {
            const auto xMidpoint = region.getXMin();
            const auto yMidpoint = region.yCenter();
            const Point2D loc = {xMidpoint, yMidpoint};
            auto labels_writer = currStyle->labelsWriter;

            auto pen = FromSpaceToViewportCoord(loc, region.getRect(), vp);
            pen.x += .5*labels_writer->getFontHeightInPixels() + y_label_xoffset_in_pixels;
            pen.y += y_label_yoffset_in_pixels;

            labels_writer->write(verticalAxisLabel, pen , currStyle->graphTitleColor, true);
        }

    }

    void AxisArtist::setHorizontalUnit(const Unit &unit) { hUnit = unit; }

    void AxisArtist::setVerticalUnit(const Unit &unit) { vUnit = unit; }

    void AxisArtist::setHorizontalAxisLabel(const Str &label) { horizontalAxisLabel = label; }

    void AxisArtist::setVerticalAxisLabel  (const Str &label) { verticalAxisLabel   = label; }

    void AxisArtist::setHorizontalAxisTicks(AxisArtist::Ticks ticks) {
        hTicksManual = true;
        hTicks = std::move(ticks);
    }

    auto AxisArtist::getHorizontalUnit() const -> const Unit & { return hUnit; }

    auto AxisArtist::getVerticalUnit() const -> const Unit & { return vUnit; }

    auto AxisArtist::getHorizontalAxisLabel() const -> Str {
        return horizontalAxisLabel;
    }

    auto AxisArtist::getVerticalAxisLabel() const -> Str {
        return verticalAxisLabel;
    }

    bool AxisArtist::hasGUI() { return true; }

    void AxisArtist::drawGUI() {
        ImGui::SliderFloat(UniqueName("x label spacing factor").c_str(), &x_spacing_multiplier, 0.5, 10.0);
        ImGui::SliderFloat(UniqueName("y label spacing factor").c_str(), &y_spacing_multiplier, 0.5, 10.0);

        IntVector axis_padding{(int)PlotThemeManager::GetCurrent()->hAxisPaddingInPixels,
                               (int)PlotThemeManager::GetCurrent()->vAxisPaddingInPixels};
        if(ImGui::SliderInt2(UniqueName("Axis padding (in pixels)").c_str(), &axis_padding[0], 20, 640)){
            PlotThemeManager::GetCurrent()->hAxisPaddingInPixels = axis_padding[0];
            PlotThemeManager::GetCurrent()->vAxisPaddingInPixels = axis_padding[1];
        };


        ImGui::SliderInt(UniqueName("y label x-padding (in pixels)").c_str(), &y_label_xoffset_in_pixels, 0, 200);
        ImGui::SliderInt(UniqueName("y label y-padding (in pixels)").c_str(), &y_label_yoffset_in_pixels, -500, 500);

        Artist::drawGUI();
    }


} // Math