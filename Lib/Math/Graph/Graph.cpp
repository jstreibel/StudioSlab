//
// Created by joao on 29/05/23.
//

#include "Graph.h"

#include <utility>
#include "imgui.h"

#include "Common/Printing.h"
#include "Core/Tools/Log.h"

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Controller/Interface/InterfaceManager.h"
#include "3rdParty/glfreetype/TextRenderer.hpp"
#include "Common/Resources.h"

#define MARK                                                                                            \
    {                                                                                                   \
    buffer.str("");                                                                                     \
    auto numRegion = log10(deltaY);                                                                     \
    buffer << std::setprecision(numRegion>2?0:numRegion>1?1:2)                                          \
           << (numRegion< -1 ? std::scientific : std::fixed) << mark;                                   \
    GLUTUtils::writeOrtho(this, {xMin,xMax,yMin,yMax}, fontScale, float(xMarkingsLabels), float(mark),  \
            buffer.str().c_str(), TICK_FONT);                                                           \
    }

Base::Graphics::Graph2D::Graph2D(Real xMin, Real xMax, Real yMin, Real yMax, Str title, bool filled, int samples)
: xMin(xMin)
, xMax(xMax)
, yMin(yMin)
, yMax(yMax)
, title(std::move(title))
, filled(filled)
, samples(samples)
{  }

Base::Graphics::Graph2D::Graph2D(Str title, bool autoReviewGraphLimits)
: title(std::move(title)), autoReviewGraphRanges(autoReviewGraphLimits) {

}

void Base::Graphics::Graph2D::_drawAxes() {

    glLineWidth(1.0);

    __computeSpacings();

    __drawXAxis();
    __drawYAxis();

    if(!labels.empty()){
        const Real Sx = (xMax-xMin) / w;
        const Real Sy = (yMax-yMin) / h;
        const Real Tx = xMin;
        const Real Ty = yMin;

        for(size_t i=0; i<labels.size(); ++i){
            labels[i]->draw(Sx, Sy, Tx, Ty);
        }
    }
}

void Base::Graphics::Graph2D::__computeSpacings() {
    {
        const Real deltaY = yMax - yMin;

        const auto theLog = log10(deltaY);
        const auto spacing = pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        yspacing = multiplier * spacing;
    }

    {
        const auto deltaX = xMax - xMin;

        const auto theLog = log10(deltaX);
        const auto spacing = pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        xspacing = multiplier * spacing;
    }
}

void Base::Graphics::Graph2D::__drawXAxis() {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    const Real inPixelsTimes2 = 5;
    const Real vTick = inPixelsTimes2 * (yMax-yMin) / h;
    const Real hTick = inPixelsTimes2 * (xMax-xMin) / w;
    (void)hTick;

    auto &gtfColor = Styles::GetColorScheme()->graphTicksFont;

    {




        static glfreetype::font_data fontData;
        static auto initd = false;
        if(!initd) {
            fontData.init(Resources::fontFileName(11).c_str(), 30);
            initd = true;
        }
        glfreetype::print(fontData, 0.1, 0.1, "Hello, frikin world!!");



        glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);

        const Real yloc = -yspacing*0.356;
        for (Real mark = 0; mark <= xMax * 1.0001; mark += xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            GLUTUtils::writeOrtho(this, {xMin, xMax, yMin, yMax}, fontScale, mark - xspacing / 18.0, yloc, buffer, TICK_FONT);
            // glfreetype::print(fontData, mark - xspacing / 18.0, yloc, "Hello, frikin world!!");
        }
        for (Real mark = 0; mark >= xMin * 1.0001; mark -= xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            GLUTUtils::writeOrtho(this, {xMin, xMax, yMin, yMax}, fontScale, mark - xspacing / 18.0, yloc, buffer, TICK_FONT);
        }
    }

    if(1)
    {
        auto &ac = Styles::GetColorScheme()->axisColor;
        auto &tc = Styles::GetColorScheme()->majorTickColor;
        glBegin(GL_LINES);
        {
            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(xMin, 0, 0);
            glVertex3d(xMax, 0, 0);

            glColor4f(tc.r, tc.g, tc.b, tc.a);

            for(Real mark = 0; mark<=xMax; mark+=xspacing){
                glVertex3d(mark, -vTick, 0);
                glVertex3d(mark, +vTick, 0);
            }
            for(Real mark = 0; mark>=xMin; mark-=xspacing){
                glVertex3d(mark, -vTick, 0);
                glVertex3d(mark, +vTick, 0);
            }
        }
        glEnd();
    }


}

void Base::Graphics::Graph2D::__drawYAxis() {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    const Real deltaY = yMax-yMin;
    const Real deltaX = xMax-xMin;
    const Real xMarkingsLabels = xMin - deltaX * 0.05;

    StringStream buffer;

    auto &gtf = Styles::GetColorScheme()->graphTicksFont;
    glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
    {
        if(yMin < 0 && yMax > 0) {
            for(Real mark = 0; mark>=yMin; mark-=yspacing)         MARK
            for(Real mark = yspacing; mark<=yMax; mark+=yspacing)  MARK
        } else {
            for (Real mark = yMin; mark <= yMax; mark += yspacing) MARK
        }
    }


    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LINE_SMOOTH);
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

void Base::Graphics::Graph2D::setupOrtho() {
    const Real deltaX = xMax-xMin;
    const Real deltaY = yMax-yMin;
    const Real xTraLeft  = -deltaX*0.07;
    const Real xTraRight = +deltaX*0.02;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xMin+xTraLeft, xMax+xTraRight, (yMin-deltaY*0.025), (yMax+deltaY*0.025), -1, 1);
}

void Base::Graphics::Graph2D::_nameLabelDraw(int i, int j, const Styles::PlotStyle &style, Str label,
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
    auto xMin = .100+(colWidth+xGap+dx)*float(j),
         xMax = xMin+dx,
         yMin = .975+(yGap+dy)*float(i),
         yMax = yMin+dy;

    if(style.filled) {
        auto color = style.fillColor;
        glColor4f(color.r, color.g, color.b, .5 * color.a);

        glRectd(xMin, yMin, xMax, yMax);
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
            glVertex2f(xMin, yMin);
            glVertex2f(xMax, yMin);
            glVertex2f(xMax, yMax);
            glVertex2f(xMin, yMax);
            glEnd();
        } else {
            glBegin(GL_LINES);
            glVertex2f(xMin, .5*(yMin+yMax));
            glVertex2f(xMax, .5*(yMin+yMax));
            glEnd();
        }

    }

    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.5);

    auto c = Styles::GetColorScheme()->graphTitleFont;
    glColor4f(c.r,c.g,c.b,c.a);
    GLUTUtils::writeOrtho(window, RectR{0,1,0,1}, fontScale, xMax+xGap, .5*(yMax+yMin), label);

    glPopMatrix();
//
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Base::Graphics::Graph2D::addLabel(Label *label) { labels.push_back(label); }

auto Base::Graphics::Graph2D::getResolution() const -> Resolution        { return samples; }

auto Base::Graphics::Graph2D::setResolution(Resolution samples_) -> void { samples = samples_; }

RectR Base::Graphics::Graph2D::getLimits() const {
    return RectR(xMin, xMax, yMin, yMax);
}

auto Base::Graphics::Graph2D::setLimits(RectR lims) -> void {
    xMin = lims.xMin;
    xMax = lims.xMax;
    yMin = lims.yMin;
    yMax = lims.yMax;
}

void Base::Graphics::Graph2D::set_xMin(Real val) { xMin = val; }
void Base::Graphics::Graph2D::set_xMax(Real val) { xMax = val; }
void Base::Graphics::Graph2D::set_yMin(Real val) { yMin = val; }
void Base::Graphics::Graph2D::set_yMax(Real val) { yMax = val; }
Real Base::Graphics::Graph2D::get_xMin() const { return xMin; }
Real Base::Graphics::Graph2D::get_xMax() const { return xMax; }
Real Base::Graphics::Graph2D::get_yMin() const { return yMin; }
Real Base::Graphics::Graph2D::get_yMax() const { return yMax; }

void
Base::Graphics::Graph2D::addPointSet(Spaces::PointSet::Ptr pointSet,
                                    Styles::PlotStyle style,
                                    Str setName,
                                    bool affectsGraphRanges) {
    auto metaData = PointSetMetadata{std::move(pointSet), style, std::move(setName), affectsGraphRanges};

    mPointSets.emplace_back(metaData);
}

void
Base::Graphics::Graph2D::addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, Str name) {
    CurveMetadata curveMetadata = {std::move(curve), style, std::move(name)};
    curves.emplace_back(curveMetadata);
}

void Base::Graphics::Graph2D::reviewGraphRanges() {
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

        // Give an extra 100*dMin% room each side.
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


void
Base::Graphics::Graph2D::_renderPointSet(const Spaces::PointSet &pSet,
                                        Styles::PlotStyle style) const noexcept {
    auto pts = pSet.getPoints();

    if(style.filled && !(style.primitive==Styles::Point))
    {
        const auto color = style.fillColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glBegin(GL_QUADS);
        {
            auto iMax = pts.size()-1;
            for(auto i=0; i<iMax; ++i){
                auto pLeft = pts[i];
                auto pRite = pts[i + 1];

                const Real xmin = pLeft.x;
                const Real xmax = pRite.x;

                const Real ymin = 0,
                        ymax1 = pLeft.y,
                        ymax2 = pRite.y;

                glVertex2d(xmin, ymin);
                glVertex2d(xmin, ymax1);
                glVertex2d(xmax, ymax2);
                glVertex2d(xmax, ymin);
            }
        }
        glEnd();
    }

    {
        glLineWidth(style.thickness);

        auto color = style.lineColor;
        glColor4f(color.r, color.g, color.b, color.a);

        if(style.primitive != Styles::SolidLine){
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else glEnable(GL_LINE_SMOOTH);

        auto primitive = GL_LINE_STRIP;
        if(style.primitive==Styles::Point){
            primitive = GL_POINTS;
            glEnable(GL_POINT_SMOOTH);
            glPointSize(style.thickness);
        }

        glBegin(primitive);
        {
            for(auto p : pts)
                glVertex2d(p.x, p.y);
        }
        glEnd();

    }
}

void Base::Graphics::Graph2D::_drawGUI() {
    auto popupName = Str("win_") + title + Str("_popup");

    if(savePopupOn) {
        ImGui::OpenPopup(popupName.c_str());
        savePopupOn = false;
    }

    if (ImGui::BeginPopup(popupName.c_str())){
        if(ImGui::MenuItem("Save graph")) {
            auto w = Printing::getTotalHorizontalDots(.5);
            auto h = w*.5;
            auto fileName = title + " " + InterfaceManager::getInstance().renderParametersToString({"N", "L"}) + ".png";
            OpenGLUtils::outputToPNG(this, fileName, w, (int)h);
        }

        ImGui::EndPopup();
    }
}

void Base::Graphics::Graph2D::_drawPointSets() {
    int i=0;
    for(auto &ptSet : mPointSets){
        auto &func = *ptSet.data;
        auto style = ptSet.plotStyle;
        auto label = ptSet.name;

        if(!label.empty()) _nameLabelDraw(i++, 0, style, label, this);

        this->_renderPointSet(func, style);
    }
}

void Base::Graphics::Graph2D::_drawCurves() {
    auto i=0;
    for(IN curveData : curves) {
        auto curve = curveData.curve;
        auto pointSet = curve.get()->renderToPointSet();
        auto points = pointSet.get()->getPoints();

        if(points.size()<2) continue;

        auto style = curveData.style;
        auto name  = curveData.name;

        _nameLabelDraw(i, 1, style, name, this);

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

        auto primitive = GL_LINE_STRIP;
        if(style.primitive==Styles::Point) primitive = GL_POINTS;

        glBegin(primitive);
        {
            for(const auto &p : points)
                glVertex2d(p.x, p.y);
        }
        glEnd();
    }
}

void Base::Graphics::Graph2D::clearPointSets() {
    mPointSets.clear();
}

void Base::Graphics::Graph2D::clearCurves() {
    curves.clear();
}

void Base::Graphics::Graph2D::draw() {
    Window::draw();

    if(autoReviewGraphRanges)
        reviewGraphRanges();

    setupOrtho();

    _drawAxes();

    _drawPointSets();

    _drawCurves();

    _drawGUI();
}

bool Base::Graphics::Graph2D::notifyMouseButton(int button, int dir, int x, int y) {
    static auto time = Timer();

    if(button == 2){
        if(dir == 0) time.reset();
        else if(dir == 1 && time.getElTime_msec() < 200) savePopupOn = true;

        return true;
    }

    return EventListener::notifyMouseButton(button, dir, x, y);
}

bool Base::Graphics::Graph2D::notifyMouseMotion(int x, int y) {
    auto elRet = EventListener::notifyMouseMotion(x, y);

    auto& mouseState = GUIBackend::GetInstance().getMouseState();

    if(mouseState.leftPressed)
    {
        const Real dxClampd = - mouseState.dx / (Real)w;
        const Real dyClampd = mouseState.dy / (Real)h;
        const Real wGraph = xMax-xMin;
        const Real hGraph = yMax-yMin;
        const Real dxGraph = wGraph * dxClampd;
        const Real dyGraph = hGraph * dyClampd;

        xMin += dxGraph;
        xMax += dxGraph;
        yMin += dyGraph;
        yMax += dyGraph;
    }
    if(mouseState.centerPressed)
    {
        constexpr const Real factor = 0.01;
        const Real dx = 1+factor*mouseState.dx;
        const Real dy = 1+factor*mouseState.dy;

        const Real x0 = .5*(xMax+xMin);
        const Real y0 = .5*(yMax+yMin);
        const Real hw = .5*(xMax-xMin)*dx;
        const Real hh = .5*(yMax-yMin)*dy;

        xMin = x0 - hw;
        xMax = x0 + hw;
        yMin = y0 - hh;
        yMax = y0 + hh;

    }

    return elRet;
}

bool Base::Graphics::Graph2D::notifyMouseWheel(int wheel, int direction, int x, int y) {
    EventListener::notifyMouseWheel(wheel, direction, x, y);

    constexpr const Real factor = 1.1;
    const Real d = pow(factor, -direction);

    if(GUIBackend::GetInstance().getMouseState().rightPressed) {
        const Real x0 = .5 * (xMax + xMin);
        const Real hw = .5 * (xMax - xMin) * d;
        xMin = x0 - hw;
        xMax = x0 + hw;
    } else {
        const Real y0 = .5 * (yMax + yMin);
        const Real hh = .5 * (yMax - yMin) * d;
        yMin = y0 - hh;
        yMax = y0 + hh;
    }

    return true;
}











