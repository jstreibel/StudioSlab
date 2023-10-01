//
// Created by joao on 29/08/23.
//

#include "Graph.h"

#include "Utils/EncodingUtils.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

#include "imgui.h"
#include "StylesManager.h"


#define hPixelsToSpaceScale (region.width() / getw())
#define vPixelsToSpaceScale (region.height() / geth())

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

Point2D FromSpaceToViewportCoord(const Point2D &spaceCoord, const RectR &spaceRegion, const RectI &viewport ) {
    fix x = spaceCoord.x;
    fix y = spaceCoord.y;
    fix xMin = spaceRegion.xMin;
    fix yMin = spaceRegion.yMin;
    fix Δx = spaceRegion.width();
    fix Δy = spaceRegion.height();
    fix W = viewport.width();
    fix H = viewport.height();

    return {W*(x-xMin)/Δx,
            H*(y-yMin)/Δy};
}

Point2D FromViewportToSpaceCoord(const Point2D &viewportCoord, const RectR &spaceRegion, const RectI &viewport ) {
    fix X = viewportCoord.x;
    fix Y = viewportCoord.y;
    fix xMin = spaceRegion.xMin;
    fix yMin = spaceRegion.yMin;
    fix Δx = spaceRegion.width();
    fix Δy = spaceRegion.height();
    fix W = viewport.width();
    fix H = viewport.height();

    return {xMin + Δx*X/W, yMin + Δy*Y/H};
}


void Core::Graphics::Graph2D::drawAxes() {

    glLineWidth(1.0);

    computeTicksSpacings();

    drawXAxis();
    OpenGLUtils::checkGLErrors(
            Str("after a call to Core::Graphics::Graph2D::drawXAxis from ") + Common::getClassName(this) + ", " +
            "graph title \"" + title + "\"");
    drawYAxis();
    OpenGLUtils::checkGLErrors(
            Str("after a call to Core::Graphics::Graph2D::drawYAxis from ") + Common::getClassName(this) + ", " +
            "graph title \"" + title + "\"");

}

Str Core::Graphics::Graph2D::getXHairLabel(const Point2D &coords) {
    fix digits = 5;
    return Str("(")+ baseHorizontalUnit(coords.x, digits) + ", " + baseVerticalUnit(coords.y, digits) + ")";
}

void Core::Graphics::Graph2D::drawXHair() {
    if(!isMouseIn()) return;

    fix vpRect = getViewport(); // getWindowRect();

    fix mouseLocal = getMouseViewportCoord();

    XHairLocation = FromViewportToSpaceCoord(mouseLocal, region, vpRect);

    auto label = getXHairLabel(XHairLocation);
    auto currStyle = Math::StylesManager::GetCurrent();
    currStyle->ticksWriter->write(label, {(Real)mouseLocal.x+20, (Real)mouseLocal.y+20});

    XHair.clear();
    XHair.addPoint({region.xMin, XHairLocation.y});
    XHair.addPoint({region.xMax, XHairLocation.y});
    XHair.addPoint({XHairLocation.x, region.yMin});
    XHair.addPoint({XHairLocation.x, region.yMax});

    Core::Graphics::Graph2D::renderPointSet(XHair, currStyle->XHairStyle);

    if(false)
    {
        auto mouse = Core::BackendManager::GetGUIBackend().getMouseState();

        ImGui::Begin("Graph debug");
        ImGui::Text("Mouse @ %i, %i", mouse.x, mouse.y);
        ImGui::Text("Mouse in %s @ %f, %f", this->title.c_str(), XHairLocation.x, XHairLocation.y);
        ImGui::Text("Local window coords: %f, %f", mouseLocal.x, mouseLocal.y);
        ImGui::Text("Viewport rect (x, y, w, h): %i, %i, %i, %i", vpRect.xMin, vpRect.yMin, vpRect.width(), vpRect.height());
        ImGui::Text("Window rect (x, y, w, h): %i, %i, %i, %i", windowRect.xMin, windowRect.yMin, windowRect.width(), windowRect.height());
        ImGui::End();
    }


}

void Core::Graphics::Graph2D::computeTicksSpacings() {
    if(1){
        const Real Δy = region.height() / baseVerticalUnit.value();

        const auto theLog = log10(Δy);
        const auto spacing = 2*pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        yspacing = multiplier * spacing;
    }

    if(1){
        const auto Δx = region.width() / baseHorizontalUnit.value();

        const auto theLog = log10(Δx);
        const auto spacing = 2*pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = 2 * floor(pow(10., theRest));

        xspacing = multiplier * spacing;
    }
}

void Core::Graphics::Graph2D::drawXAxis() {
    auto currStyle = Math::StylesManager::GetCurrent();

    auto writer = currStyle->ticksWriter;

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
        auto &gtfColor = currStyle->graphNumbersColor;

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);

        for (Real mark = 0; mark <= region.xMax * 1.0001; mark += xspacing)
        {
            Point2D loc = {mark - xspacing / 18.0, yLocationOfLabels};
            loc = FromSpaceToViewportCoord(loc, region, getViewport());
            auto label = baseHorizontalUnit(mark, 2);
            //auto label = ToStr(mark, 2)
            writer->write(label, loc, gtfColor);
        }
        for (Real mark = -xspacing; mark >= region.xMin * 1.0001; mark -= xspacing) {
            Point2D loc = {mark - xspacing / 18.0, yLocationOfLabels};
            loc = FromSpaceToViewportCoord(loc, region, getViewport());
            auto label = baseHorizontalUnit(mark, 2);
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

            glVertex3d(region.xMin, yLocationOfXAxis, 0);
            glVertex3d(region.xMax, yLocationOfXAxis, 0);

            glColor4f(tc.r, tc.g, tc.b, tc.a);

            Count size = (Count) region.xMax/xspacing;
            for(auto i = 0; i<=size; ++i){
                Real mark = i*xspacing;
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

    OpenGLUtils::checkGLErrors(Str(__PRETTY_FUNCTION__));
}

void Core::Graphics::Graph2D::drawYAxis() {
    auto currStyle = Math::StylesManager::GetCurrent();

    auto writer = currStyle->ticksWriter;

    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    fix Δy = region.height();
    fix xLocationOfYAxis = region.xMin + 20*hPixelsToSpaceScale;
    fix yOffsetOfLabels = 0.2*writer->getFontHeightInPixels()* vPixelsToSpaceScale;

    StringStream buffer;

    auto &gtf = currStyle->graphNumbersColor;
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

void Core::Graphics::Graph2D::nameLabelDraw(const Styles::PlotStyle &style, const Str& label) {
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