//
// Created by joao on 29/08/23.
//

#include "Graph.h"

#include "Core/Tools/Log.h"
#include "Utils/EncodingUtils.h"

#include "Core/Backend/GUIBackend.h"


#define vPixelsToSpaceScale (region.height() / geth());
#define hPixelsToSpaceScale (region.width() / getw());

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
        loc = FromSpaceToViewportCoord(loc, region, getViewport());           \
        writer->write(text, loc, gtf);                                        \
                                                                              \
    }

Point2D FromSpaceToViewportCoord(const Point2D &spaceCoord, RectR spaceRegion, RectI viewport ) {
    fix x = spaceCoord.x;
    fix y = spaceCoord.y;
    fix xMin = spaceRegion.xMin;
    fix yMin = spaceRegion.yMin;
    fix Δx = spaceRegion.width();
    fix Δy = spaceRegion.height();
    fix W = viewport.width();
    fix H = viewport.height();

    return {W*(x-xMin)/Δx, H*(y-yMin)/Δy};
}


void Core::Graphics::Graph2D::drawAxes() {

    glLineWidth(1.0);

    computeTicksSpacings();

    drawXAxis();
    drawYAxis();

}

void Core::Graphics::Graph2D::drawXHair() {
    if(!gotHit) return;

    fix &mouse = GUIBackend::GetInstance().getMouseState();
    // GUIBackend::GetInstance().getSystemWindowData();
    fix xMouse = mouse.x;
    fix yMouse = mouse.y;

}

void Core::Graphics::Graph2D::computeTicksSpacings() {
    if(1){
        const Real Δy = region.height();

        const auto theLog = log10(Δy);
        const auto spacing = 2*pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        yspacing = multiplier * spacing;
    }

    if(1){
        const auto Δx = region.width();

        const auto theLog = log10(Δx);
        const auto spacing = 2*pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        xspacing = multiplier * spacing;
    }
}

void Core::Graphics::Graph2D::drawXAxis() {
    auto *writer = Styles::GetCurrent()->ticksWriter;

    fix vTickHeightInSpace = vTickHeightinPixels_x2 * vPixelsToSpaceScale;
    fix hTickHeightInSpace = vTickHeightinPixels_x2 * hPixelsToSpaceScale;
    (void)hTickHeightInSpace;

    fix vGraphPaddingInSpace = vGraphPaddingInPixels * vPixelsToSpaceScale;
    fix fontHeight = writer->getFontHeightInPixels();

    fix yLocationOfXAxis = region.yMin < -vGraphPaddingInSpace
                                ? 0
                                : region.yMin + vGraphPaddingInSpace;
    fix yLocationOfLabels = yLocationOfXAxis -1.1 * (vTickHeightInSpace+fontHeight) * vPixelsToSpaceScale;
    {
        auto &gtfColor = Styles::GetCurrent()->graphTicksFont;

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);

        for (Real mark = 0; mark <= region.xMax * 1.0001; mark += xspacing)
        // for (int m=0; m<xMax/xspacing; ++m)
        {
            Point2D loc = {mark - xspacing / 18.0, yLocationOfLabels};
            loc = FromSpaceToViewportCoord(loc, region, getViewport());
            writer->write(ToStr(mark, 2), loc, gtfColor);
        }
        for (Real mark = -xspacing; mark >= region.xMin * 1.0001; mark -= xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);

            Point2D loc = {mark - xspacing / 18.0, yLocationOfLabels};
            loc = FromSpaceToViewportCoord(loc, region, getViewport());
            writer->write(Str(buffer), loc, gtfColor);
        }
    }

    if(1)
    {
        auto &ac = Styles::GetCurrent()->axisColor;
        auto &tc = Styles::GetCurrent()->majorTickColor;
        glBegin(GL_LINES);
        {
            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(region.xMin, yLocationOfXAxis, 0);
            glVertex3d(region.xMax, yLocationOfXAxis, 0);

            glColor4f(tc.r, tc.g, tc.b, tc.a);

            for(Real mark = 0; mark<=region.xMax; mark+=xspacing){
                glVertex3d(mark, -vTickHeightInSpace, 0);
                glVertex3d(mark, +vTickHeightInSpace, 0);
            }
            for(Real mark = 0; mark>=region.xMin; mark-=xspacing){
                glVertex3d(mark, -vTickHeightInSpace, 0);
                glVertex3d(mark, +vTickHeightInSpace, 0);
            }
        }
        glEnd();
    }
}

void Core::Graphics::Graph2D::drawYAxis() {
    auto *writer = Styles::GetCurrent()->ticksWriter;

    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    fix Δy = region.height();
    fix xLocationOfYAxis = region.xMin + 20*hPixelsToSpaceScale;
    fix yOffsetOfLabels = 0.2*writer->getFontHeightInPixels()* vPixelsToSpaceScale;

    StringStream buffer;

    auto &gtf = Styles::GetCurrent()->graphTicksFont;
    glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
    {
        auto numRegion = log10(Δy);

        if(region.yMin < 0 && region.yMax > 0) {
            for(Real mark = 0; mark>=region.yMin; mark-=yspacing)         MARK_Y
            for(Real mark = yspacing; mark<=region.yMax; mark+=yspacing)  MARK_Y
        } else {
            for (Real mark = region.yMin; mark <= region.yMax; mark += yspacing) MARK_Y
        }
    }

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LINE_SMOOTH);

    glDisable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    {
        auto &ac = Styles::GetCurrent()->axisColor;

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
        auto &ac = Styles::GetCurrent()->axisColor;
        auto &tc = Styles::GetCurrent()->majorTickColor;

        glColor4f(tc.r, tc.g, tc.b, tc.a);

        if(region.yMin < 0 && region.yMax > 0) {
            for(Real mark = 0; mark>=region.yMin; mark-=yspacing) {
                glVertex3d(region.xMin, mark, 0);
                glVertex3d(region.xMax, mark, 0);
            }
            for(Real mark = yspacing; mark<=region.yMax; mark+=yspacing) {
                glVertex3d(region.xMin, mark, 0);
                glVertex3d(region.xMax, mark, 0);
            }
        } else {
            for (Real mark = region.yMin; mark <= region.yMax; mark += yspacing) {
                glVertex3d(region.xMin, mark, 0);
                glVertex3d(region.xMax, mark, 0);
            }
        }

        glColor4f(ac.r, ac.g, ac.b, ac.a);

        glVertex3d(region.xMin, 0, 0);
        glVertex3d(region.xMax, 0, 0);
    }
    glEnd();
    glPopAttrib();

}

void Core::Graphics::Graph2D::reviewGraphRanges() {
    if(!mPointSets.empty())
    {
        auto referencePointSet = mPointSets[0].data;

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

void Core::Graphics::Graph2D::nameLabelDraw(const Styles::PlotStyle &style, const Str& label) {
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

    auto c = Styles::GetCurrent()->graphTitleFont;
    Point2D loc = {xMax_label + xGap, .5 * (yMax_label + yMin_label)};

    auto *writer = Styles::GetCurrent()->labelsWriter;
    loc = FromSpaceToViewportCoord(loc, {0, 1, 0, 1}, getViewport());
    writer->write(label, {loc.x, loc.y}, c);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}