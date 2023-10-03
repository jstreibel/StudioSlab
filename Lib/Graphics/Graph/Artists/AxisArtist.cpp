//
// Created by joao on 1/10/23.
//

#include "Utils/EncodingUtils.h"

#include "Graphics/Graph/Graph.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/Utils.h"
#include "Graphics/OpenGL/Shader.h"

#include "AxisArtist.h"
#include "Graphics/Graph/StylesManager.h"


#define hPixelsToSpaceScale (region.width() / vp.width())
#define vPixelsToSpaceScale (region.height() / vp.height())

#define setupBuffer(buffer) \
    if     (numRegion> +2)  (buffer) << std::setprecision(0); \
    else if(numRegion> +1)  (buffer) << std::setprecision(1); \
    else                    (buffer) << std::setprecision(2); \
                                                              \
    if     (numRegion< -1)  (buffer) << std::scientific;      \
    else                    (buffer) << std::fixed;           \
                                                              \
    if     (mark < 0)       (buffer) << "";                   \
    else (buffer) << " ";


namespace Graphics {

    AxisArtist::AxisArtist()
    : AxisArtist(Constants::One, Constants::One)
    {    }

    AxisArtist::AxisArtist(const Unit& horizontal, const Unit& vertical)
    : hUnit(horizontal)
    , vUnit(vertical)
    {    }

    void AxisArtist::draw(const Graph2D &graph) {
        glLineWidth(1.0);

        computeTicksSpacings(graph);

        drawXAxis(graph);
        drawYAxis(graph);
    }

    void AxisArtist::computeTicksSpacings(const Graph2D &graph) {
        auto &region = graph.getRegion();

        if(true){
            const Real Δy = region.height() / vUnit.value();

            const auto theLog = log10(Δy);
            const auto spacing = 2*pow(10., floor(theLog) - 1.);
            const auto theRest = theLog - floor(theLog);
            const auto multiplier = floor(pow(10., theRest));

            ySpacing = multiplier * spacing;
        }

        if(true){
            const auto Δx = region.width() / hUnit.value();

            const auto theLog = log10(Δx);
            const auto spacing = 2*pow(10., floor(theLog) - 1.);
            const auto theRest = theLog - floor(theLog);
            const auto multiplier = 2 * floor(pow(10., theRest));

            xSpacing = multiplier * spacing;
        }
    }

    void AxisArtist::drawXAxis(const Graph2D &graph) const {
        auto vp = graph.getViewport();
        auto region = graph.getRegion();

        fix iMin = int(region.xMin/xSpacing);
        fix iMax = int(region.xMax/xSpacing);

        auto currStyle = Math::StylesManager::GetCurrent();

        auto writer = currStyle->ticksWriter;

        fix vTickHeightinPixels_x2 = 2.0*(Real)currStyle->vTickHeightinPixels;
        fix vAxisPaddingInPixels = (Real)currStyle->vAxisPaddingInPixels;

        fix vTickHeightInSpace = vTickHeightinPixels_x2 * vPixelsToSpaceScale;
        fix hTickHeightInSpace = vTickHeightinPixels_x2 * hPixelsToSpaceScale;
        (void)hTickHeightInSpace;

        fix vGraphPaddingInSpace = vAxisPaddingInPixels * vPixelsToSpaceScale;
        fix fontHeight = writer->getFontHeightInPixels();

        fix yLocationOfXAxis = region.yMin < -vGraphPaddingInSpace
                               ? 0
                               : region.yMin + vGraphPaddingInSpace;
        fix yLocationOfLabels = yLocationOfXAxis -1.1 * (vTickHeightInSpace+fontHeight) * vPixelsToSpaceScale;

        auto &gtfColor = currStyle->graphNumbersColor;

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);

        for (auto i=iMin; i<=iMax; ++i) {
            fix mark = i*xSpacing;

            Point2D loc = {mark - xSpacing / 18.0, yLocationOfLabels};
            loc = FromSpaceToViewportCoord(loc, region, vp);
            auto label = hUnit(mark, 2);
            writer->write(label, loc, gtfColor);
        }

        {
            OpenGL::Shader::remove();

            auto &ac = currStyle->axisColor;
            auto &tc = currStyle->majorTickColor;
            glBegin(GL_LINES);
            {
                glColor4f(ac.r, ac.g, ac.b, ac.a);

                glVertex3d(region.xMin, yLocationOfXAxis, 0);
                glVertex3d(region.xMax, yLocationOfXAxis, 0);

                glColor4f(tc.r, tc.g, tc.b, tc.a);

                for(auto i = iMin; i<=iMax; ++i){
                    Real mark = i*xSpacing;
                    glVertex3d(mark, -vTickHeightInSpace, 0);
                    glVertex3d(mark, +vTickHeightInSpace, 0);
                }
            }
            glEnd();
        }

        Graphics::OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__));
    }

    void AxisArtist::drawYAxis(const Graph2D &graph) const {
        auto vp = graph.getViewport();
        auto region = graph.getRegion();
        auto currStyle = Math::StylesManager::GetCurrent();

        auto writer = currStyle->ticksWriter;

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        fix Δy = region.height();
        fix xLocationOfYAxis = region.xMin + 20*hPixelsToSpaceScale;
        fix yOffsetOfLabels = 0.2*writer->getFontHeightInPixels()* vPixelsToSpaceScale;
        fix iMin = int(region.yMin/ySpacing);
        fix iMax = int(region.yMax/ySpacing);

        StringStream buffer;

        auto &gtf = currStyle->graphNumbersColor;
        glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
        auto numRegion = log10(Δy);

        for(int i=iMin; i<=iMax; ++i){
            auto mark = i*ySpacing;

            Point2D loc = {float(xLocationOfYAxis), float(mark)+yOffsetOfLabels};
            buffer.str("");

            setupBuffer(buffer);

            buffer << mark;

            Str text = buffer.str();
            if(numRegion < -1) text = elegantScientific(text);

            loc = FromSpaceToViewportCoord(loc, region, vp);
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

            glVertex3d(region.xMin, 0, 0);
            glVertex3d(region.xMax, 0, 0);
        }
        glEnd();

        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0x2727);
        glLineStipple(2, 0x1249);
        glLineStipple(2, 0x1111);

        glBegin(GL_LINES);
        {
            auto &ac = currStyle->axisColor;
            auto &tc = currStyle->gridLines.lineColor;

            glColor4f(tc.r, tc.g, tc.b, tc.a);

            for(auto i = iMin; i<=iMax; ++i) {
                Real mark = i*ySpacing;
                glVertex3d(region.xMin, mark, 0);
                glVertex3d(region.xMax, mark, 0);
            }

            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(region.xMin, 0, 0);
            glVertex3d(region.xMax, 0, 0);
        }
        glEnd();
        glPopAttrib();

    }

    void AxisArtist::setHorizontalUnit(const Unit &unit) { hUnit = unit; }

    void AxisArtist::setVerticalUnit(const Unit &unit) { vUnit = unit; }



} // Math