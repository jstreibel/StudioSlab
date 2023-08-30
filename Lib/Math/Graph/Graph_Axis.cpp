//
// Created by joao on 29/08/23.
//

#include "Graph.h"

#include "Graph_Config.h"


#define vPixelsToSpaceScale ((yMax - yMin) / h);
#define hPixelsToSpaceScale ((xMax - xMin) / w);

fix vTickHeightinPixels_x2 = 5;
fix vGraphPaddingInPixels = 60;


#define MARK_Y                                                                                                  \
    {                                                                                                           \
        Point2D loc = {float(xLocationOfYAxis), float(mark)+yOffsetOfLabels};                                   \
        buffer.str("");                                                                                         \
        buffer << std::setprecision(numRegion>2?0:numRegion>1?1:2)                                              \
            << (numRegion< -1 ? std::scientific : std::fixed) << (mark<0?"":" ") << mark;                       \
        RectR region = {xMin, xMax, yMin, yMax};                                                                \
        if(USE_FREETYPE_GL){                                                                                    \
            loc = FromSpaceToViewportCoord(loc, region, getViewport());                                         \
            writer.write(buffer.str(), loc, gtf);                                                               \
        }                                                                                                       \
        else{                                                                                                   \
            GLUTUtils::writeOrtho(this, region, fontScale, loc.x, loc.y,                                        \
                buffer.str().c_str(), TICK_FONT);                                                               \
        }                                                                                                       \
    }


Point2D FromSpaceToViewportCoord(const Point2D &spaceCoord, RectR spaceRegion, RectI viewport ) {
    fix x = spaceCoord.x;
    fix y = spaceCoord.y;
    fix xMin = spaceRegion.xMin;
    fix yMin = spaceRegion.yMin;
    fix Δx = spaceRegion.w();
    fix Δy = spaceRegion.h();
    fix W = viewport.w();
    fix H = viewport.h();

    return {W*(x-xMin)/Δx, H*(y-yMin)/Δy};
}


void Core::Graphics::Graph2D::drawAxes() {

    glLineWidth(1.0);

    computeTicksSpacings();

    drawXAxis();
    drawYAxis();

}

void Core::Graphics::Graph2D::computeTicksSpacings() {
    if(1){
        const Real Δy = yMax - yMin;

        const auto theLog = log10(Δy);
        const auto spacing = pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        yspacing = multiplier * spacing;
    }

    if(1){
        const auto Δx = xMax - xMin;

        const auto theLog = log10(Δx);
        const auto spacing = pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        xspacing = multiplier * spacing;
    }
}

void Core::Graphics::Graph2D::drawXAxis() {
    fix vTickHeightInSpace = vTickHeightinPixels_x2 * vPixelsToSpaceScale;
    fix hTickHeightInSpace = vTickHeightinPixels_x2 * hPixelsToSpaceScale;
    (void)hTickHeightInSpace;

    fix vGraphPaddingInSpace = vGraphPaddingInPixels * vPixelsToSpaceScale;
    fix fontHeight = writer.getFontHeightInPixels();

    fix yLocationOfXAxis = yMin < -vGraphPaddingInSpace
                                ? 0
                                : yMin + vGraphPaddingInSpace;
    fix yLocationOfLabels = yLocationOfXAxis -1.1 * (vTickHeightInSpace+fontHeight) * vPixelsToSpaceScale;
    {
        auto &gtfColor = Styles::GetColorScheme()->graphTicksFont;

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);
        if(!USE_FREETYPE_GL) glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);



        for (Real mark = 0; mark <= xMax * 1.0001; mark += xspacing)
        // for (int m=0; m<xMax/xspacing; ++m)
        {
            // auto mark = 0.0 + m*xspacing;

            char buffer[64];
            sprintf(buffer, "%.2f", mark);

            if(USE_FREETYPE_GL) {
                Point2D loc = {mark - xspacing / 18.0, yLocationOfLabels};
                RectR region = {xMin, xMax, yMin, yMax};
                loc = FromSpaceToViewportCoord(loc, region, getViewport());
                writer.write(Str(buffer), loc, gtfColor);
            } else {
                glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);
                GLUTUtils::writeOrtho(this, {xMin, xMax, yMin, yMax}, fontScale, mark - xspacing / 18.0, yLocationOfXAxis, buffer,
                                      TICK_FONT);
            }
        }
        for (Real mark = -xspacing; mark >= xMin * 1.0001; mark -= xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);

            if(USE_FREETYPE_GL) {
                Point2D loc = {mark - xspacing / 18.0, yLocationOfLabels};
                RectR region = {xMin, xMax, yMin, yMax};
                loc = FromSpaceToViewportCoord(loc, region, getViewport());
                writer.write(Str(buffer), loc, gtfColor);
            } else {
                GLUTUtils::writeOrtho(this, {xMin, xMax, yMin, yMax}, fontScale, mark - xspacing / 18.0, yLocationOfXAxis, buffer,
                                      TICK_FONT);
            }
        }
    }

    if(1)
    {
        auto &ac = Styles::GetColorScheme()->axisColor;
        auto &tc = Styles::GetColorScheme()->majorTickColor;
        glBegin(GL_LINES);
        {
            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(xMin, yLocationOfXAxis, 0);
            glVertex3d(xMax, yLocationOfXAxis, 0);

            glColor4f(tc.r, tc.g, tc.b, tc.a);

            for(Real mark = 0; mark<=xMax; mark+=xspacing){
                glVertex3d(mark, -vTickHeightInSpace, 0);
                glVertex3d(mark, +vTickHeightInSpace, 0);
            }
            for(Real mark = 0; mark>=xMin; mark-=xspacing){
                glVertex3d(mark, -vTickHeightInSpace, 0);
                glVertex3d(mark, +vTickHeightInSpace, 0);
            }
        }
        glEnd();
    }
}

void Core::Graphics::Graph2D::drawYAxis() {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    fix Δy = yMax-yMin;
    fix Δx = xMax-xMin;
    fix xLocationOfYAxis = xMin + 20*hPixelsToSpaceScale;
    fix yOffsetOfLabels = 0.2*writer.getFontHeightInPixels()*vPixelsToSpaceScale;

    StringStream buffer;

    auto &gtf = Styles::GetColorScheme()->graphTicksFont;
    glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
    {
        auto numRegion = log10(Δy);

        if(yMin < 0 && yMax > 0) {
            for(Real mark = 0; mark>=yMin; mark-=yspacing)         MARK_Y
            for(Real mark = yspacing; mark<=yMax; mark+=yspacing)  MARK_Y
        } else {
            for (Real mark = yMin; mark <= yMax; mark += yspacing) MARK_Y
        }
    }

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LINE_SMOOTH);

    glDisable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    {
        auto &ac = Styles::GetColorScheme()->axisColor;

        glColor4f(ac.r, ac.g, ac.b, ac.a);

        glVertex3d(xMin, 0, 0);
        glVertex3d(xMax, 0, 0);
    }
    glEnd();

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x2727);
    glLineStipple(2, 0x1249);
    glLineStipple(2, 0x1111);

    glBegin(GL_LINES);
    {
        auto &ac = Styles::GetColorScheme()->axisColor;
        auto &tc = Styles::GetColorScheme()->majorTickColor;

        glColor4f(tc.r, tc.g, tc.b, tc.a);

        if(yMin < 0 && yMax > 0) {
            for(Real mark = 0; mark>=yMin; mark-=yspacing) {
                glVertex3d(xMin, mark, 0);
                glVertex3d(xMax, mark, 0);
            }
            for(Real mark = yspacing; mark<=yMax; mark+=yspacing) {
                glVertex3d(xMin, mark, 0);
                glVertex3d(xMax, mark, 0);
            }
        } else {
            for (Real mark = yMin; mark <= yMax; mark += yspacing) {
                glVertex3d(xMin, mark, 0);
                glVertex3d(xMax, mark, 0);
            }
        }

        glColor4f(ac.r, ac.g, ac.b, ac.a);

        glVertex3d(xMin, 0, 0);
        glVertex3d(xMax, 0, 0);
    }
    glEnd();
    glPopAttrib();

}

void Core::Graphics::Graph2D::reviewGraphRanges() {
    if(!mPointSets.empty())
    {
        auto referencePointSet = mPointSets[0].data;

        xMax = referencePointSet->getMax().x;
        xMin = referencePointSet->getMin().x;
        yMax = referencePointSet->getMax().y;
        yMin = referencePointSet->getMin().y;

        for (auto &set: mPointSets) {
            if(!set.affectsGraphRanges) continue;

            auto max = set.data->getMax();
            auto min = set.data->getMin();

            if (max.x > xMax) xMax = max.x;
            if (min.x < xMin) xMin = min.x;
            if (max.y > yMax) yMax = max.y;
            if (min.y < yMin) yMin = min.y;
        }

        if(true)
        {
            const auto dMin = .025;

            auto dx = dMin * (xMax - xMin);
            auto dy = dMin * (yMax - yMin);

            if(dx == 0) dx = dMin;
            if(dy == 0) dy = dMin;

            xMax += dx;
            xMin -= dx;
            yMax += dy;
            yMin -= dy;
        }
    }
}

void Core::Graphics::Graph2D::nameLabelDraw(int i, int j, const Styles::PlotStyle &style, Str label,
                                            const Window *window) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    auto dx = .080,
            dy = -.060;
    auto xGap = 0.015,
            yGap = -.025;
    auto colWidth = 0.5;
    auto xMin_label = .100 + (colWidth + xGap + dx) * float(j),
            xMax_label = xMin_label + dx,
            yMin_label = .975 + (yGap + dy) * float(i),
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

    auto c = Styles::GetColorScheme()->graphTitleFont;
    glColor4f(c.r,c.g,c.b,c.a);
    Point2D loc = {xMax_label + xGap, .5 * (yMax_label + yMin_label)};

    if(USE_FREETYPE_GL) {
        loc = FromSpaceToViewportCoord(loc, {0, 1, 0, 1}, getViewport());
        writer.write(label, {loc.x, loc.y});
    } else {
        GLUTUtils::writeOrtho(window, RectR{0, 1, 0, 1}, fontScale, loc.x, loc.y, label);
    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}