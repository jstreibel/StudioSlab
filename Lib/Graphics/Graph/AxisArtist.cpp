//
// Created by joao on 1/10/23.
//

#include "Utils/EncodingUtils.h"

#include "Graphics/Graph/Graph.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/Utils.h"
#include "Graphics/OpenGL/Shader.h"

#include "AxisArtist.h"
#include "StylesManager.h"


#define hPixelsToSpaceScale (tiedRegion.width() / vp.width())
#define vPixelsToSpaceScale (tiedRegion.height() / vp.height())

fix vTickHeightinPixels_x2 = 5;
fix vGraphPaddingInPixels = 60;


#define MARK_Y                                                                \
    {                                                                         \
        Point2D loc = {float(xLocationOfYAxis), float(mark)+yOffsetOfLabels}; \
        buffer.str("");                                                       \
                                                                              \
        if(numRegion>2)       buffer << std::setprecision(0);                 \
        else if(numRegion>1)  buffer << std::setprecision(1);                 \
        else                  buffer << std::setprecision(2);                 \
                                                                              \
        if(numRegion < -1)    buffer << std::scientific;                      \
        else                  buffer << std::fixed;                           \
                                                                              \
        if(mark < 0)          buffer << "";                                   \
        else buffer << " ";                                                   \
                                                                              \
        buffer << mark;                                                       \
                                                                              \
        Str text = buffer.str();                                              \
        if(numRegion < -1) text = elegantScientific(text);                    \
                                                                              \
                                                                              \
        loc = FromSpaceToViewportCoord(loc, tiedRegion, vp);                  \
        writer->write(text, loc, gtf);                                        \
                                                                              \
    }


namespace Graphics {

    AxisArtist::AxisArtist(const RectR &regionToTie, const Unit& horizontal, const Unit& vertical)
    : tiedRegion(regionToTie)
    , hUnit(horizontal)
    , vUnit(vertical)
    {    }

    void AxisArtist::draw(const Graph2D &graph) {
        auto viewport = graph.getViewport();

        glLineWidth(1.0);

        computeTicksSpacings();

        drawXAxis(viewport);
        drawYAxis(viewport);
    }

    void AxisArtist::computeTicksSpacings() {
        if(true){
            const Real Δy = tiedRegion.height() / vUnit.value();

            const auto theLog = log10(Δy);
            const auto spacing = 2*pow(10., floor(theLog) - 1.);
            const auto theRest = theLog - floor(theLog);
            const auto multiplier = floor(pow(10., theRest));

            ySpacing = multiplier * spacing;
        }

        if(true){
            const auto Δx = tiedRegion.width() / hUnit.value();

            const auto theLog = log10(Δx);
            const auto spacing = 2*pow(10., floor(theLog) - 1.);
            const auto theRest = theLog - floor(theLog);
            const auto multiplier = 2 * floor(pow(10., theRest));

            xSpacing = multiplier * spacing;
        }
    }

    void AxisArtist::drawXAxis(const RectI &vp) {
        auto currStyle = Math::StylesManager::GetCurrent();

        auto writer = currStyle->ticksWriter;

        fix vTickHeightInSpace = vTickHeightinPixels_x2 * vPixelsToSpaceScale;
        fix hTickHeightInSpace = vTickHeightinPixels_x2 * hPixelsToSpaceScale;
        (void)hTickHeightInSpace;

        fix vGraphPaddingInSpace = vGraphPaddingInPixels * vPixelsToSpaceScale;
        fix fontHeight = writer->getFontHeightInPixels();

        fix yLocationOfXAxis = tiedRegion.yMin < -vGraphPaddingInSpace
                               ? 0
                               : tiedRegion.yMin + vGraphPaddingInSpace;
        fix yLocationOfLabels = yLocationOfXAxis -1.1 * (vTickHeightInSpace+fontHeight) * vPixelsToSpaceScale;
        {
            auto &gtfColor = currStyle->graphNumbersColor;

            glEnable(GL_LINE_SMOOTH);
            glDisable(GL_LINE_STIPPLE);

            glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);

            for (Real mark = 0; mark <= tiedRegion.xMax * 1.0001; mark += xSpacing)
            {
                Point2D loc = {mark - xSpacing / 18.0, yLocationOfLabels};
                loc = FromSpaceToViewportCoord(loc, tiedRegion, vp);
                auto label = hUnit(mark, 2);
                //auto label = ToStr(mark, 2)
                writer->write(label, loc, gtfColor);
            }
            for (Real mark = -xSpacing; mark >= tiedRegion.xMin * 1.0001; mark -= xSpacing) {
                Point2D loc = {mark - xSpacing / 18.0, yLocationOfLabels};
                loc = FromSpaceToViewportCoord(loc, tiedRegion, vp);
                auto label = hUnit(mark, 2);
                //auto label = ToStr(mark, 2)
                writer->write(label, loc, gtfColor);
            }
        }

        if(1)
        {
            OpenGL::Shader::remove();

            auto &ac = currStyle->axisColor;
            auto &tc = currStyle->majorTickColor;
            glBegin(GL_LINES);
            {
                glColor4f(ac.r, ac.g, ac.b, ac.a);

                glVertex3d(tiedRegion.xMin, yLocationOfXAxis, 0);
                glVertex3d(tiedRegion.xMax, yLocationOfXAxis, 0);

                glColor4f(tc.r, tc.g, tc.b, tc.a);

                Count size = (Count) (tiedRegion.xMax/xSpacing);
                for(auto i = 0; i<=size; ++i){
                    Real mark = i*xSpacing;
                    glVertex3d(mark, -vTickHeightInSpace, 0);
                    glVertex3d(mark, +vTickHeightInSpace, 0);
                }
                for(Real mark = 0; mark>=tiedRegion.xMin; mark-=xSpacing){
                    glVertex3d(mark, -vTickHeightInSpace, 0);
                    glVertex3d(mark, +vTickHeightInSpace, 0);
                }
            }
            glEnd();
        }

        Graphics::OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__));
    }

    void AxisArtist::drawYAxis(const RectI &vp) {
        auto currStyle = Math::StylesManager::GetCurrent();

        auto writer = currStyle->ticksWriter;

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        fix Δy = tiedRegion.height();
        fix xLocationOfYAxis = tiedRegion.xMin + 20*hPixelsToSpaceScale;
        fix yOffsetOfLabels = 0.2*writer->getFontHeightInPixels()* vPixelsToSpaceScale;

        StringStream buffer;

        auto &gtf = currStyle->graphNumbersColor;
        glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
        {
            auto numRegion = log10(Δy);

            if(tiedRegion.yMin < 0 && tiedRegion.yMax > 0) {
                for(Real mark = 0; mark>=tiedRegion.yMin; mark-=ySpacing)                    MARK_Y
                        for(Real mark = ySpacing; mark<=tiedRegion.yMax; mark+=ySpacing)     MARK_Y
            } else {
                for (Real mark = tiedRegion.yMin; mark <= tiedRegion.yMax; mark += ySpacing) MARK_Y
            }
        }

        OpenGL::Shader::remove();

        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LINE_SMOOTH);

        glDisable(GL_LINE_STIPPLE);
        glBegin(GL_LINES);
        {
            auto &ac = currStyle->axisColor;

            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(tiedRegion.xMin, 0, 0);
            glVertex3d(tiedRegion.xMax, 0, 0);
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

            if(tiedRegion.yMin < 0 && tiedRegion.yMax > 0) {
                for(Real mark = 0; mark>=tiedRegion.yMin; mark-=ySpacing) {
                    glVertex3d(tiedRegion.xMin, mark, 0);
                    glVertex3d(tiedRegion.xMax, mark, 0);
                }
                for(Real mark = ySpacing; mark<=tiedRegion.yMax; mark+=ySpacing) {
                    glVertex3d(tiedRegion.xMin, mark, 0);
                    glVertex3d(tiedRegion.xMax, mark, 0);
                }
            } else {
                for (Real mark = tiedRegion.yMin; mark <= tiedRegion.yMax; mark += ySpacing) {
                    glVertex3d(tiedRegion.xMin, mark, 0);
                    glVertex3d(tiedRegion.xMax, mark, 0);
                }
            }

            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(tiedRegion.xMin, 0, 0);
            glVertex3d(tiedRegion.xMax, 0, 0);
        }
        glEnd();
        glPopAttrib();

    }

    void AxisArtist::setHorizontalUnit(const Unit &unit) { hUnit = unit; }

    void AxisArtist::setVerticalUnit(const Unit &unit) { vUnit = unit; }

} // Math