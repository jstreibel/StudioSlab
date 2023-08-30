//
// Created by joao on 29/05/23.
//

#include "Graph.h"

#include <utility>
#include "imgui.h"

#include "Utils/Printing.h"
#include "Core/Tools/Log.h"

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Controller/Interface/InterfaceManager.h"
#include "3rdParty/glfreetype/TextRenderer.hpp"
#include "Utils/Resources.h"

#include "Graph_Config.h"

std::map<Str, Core::Graphics::Graph2D*> Core::Graphics::Graph2D::graphMap = {};

Core::Graphics::Graph2D::Graph2D(Real xMin, Real xMax, Real yMin, Real yMax, Str _title, int samples)
: writer(Resources::fontFileName(FONT_INDEX), FONT_SIZE)
, xMin(xMin)
, xMax(xMax)
, yMin(yMin)
, yMax(yMax)
, title(std::move(_title))
, samples(samples)
{
    if(title.empty()) title = Str("unnamed");
    Count n=1;
    while(Graph2D::graphMap.count(title))
        title += " (" + ToStr(++n) + ")";
    Graph2D::graphMap[title] = this;

    Log::Note() << "Created Graph2D '" << title << "'" << Log::Flush;

    fix vp = getViewport();
    writer.reshape(vp.w(), vp.h());
}

Core::Graphics::Graph2D::Graph2D(Str title, bool autoReviewGraphLimits)
: Graph2D(-1,1,-1,1,std::move(title))
{
    autoReviewGraphRanges = autoReviewGraphLimits;
}

void Core::Graphics::Graph2D::draw() {
    Window::draw();

    if (autoReviewGraphRanges) reviewGraphRanges();

    setupOrtho();

    drawAxes();

    drawPointSets();

    drawCurves();

    drawGUI();

}

void
Core::Graphics::Graph2D::addPointSet(Spaces::PointSet::Ptr pointSet,
                                     Styles::PlotStyle style,
                                     Str setName,
                                     bool affectsGraphRanges) {
    auto metaData = PointSetMetadata{std::move(pointSet), style, std::move(setName), affectsGraphRanges};

    mPointSets.emplace_back(metaData);
}

void
Core::Graphics::Graph2D::addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, Str name) {
    CurveMetadata curveMetadata = {std::move(curve), style, std::move(name)};
    curves.emplace_back(curveMetadata);
}

void
Core::Graphics::Graph2D::renderPointSet(const Spaces::PointSet &pSet,
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

void Core::Graphics::Graph2D::drawGUI() {
    auto popupName = Str("win_") + title + Str("_popup");

    if(savePopupOn) {
        Log::Info() << "Popup '" << popupName << "' is on" << Log::Flush;
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

void Core::Graphics::Graph2D::drawPointSets() {
    int i=0;
    for(auto &ptSet : mPointSets){
        auto &func = *ptSet.data;
        auto style = ptSet.plotStyle;
        auto label = ptSet.name;

        if(!label.empty()) nameLabelDraw(i++, 0, style, label, this);

        this->renderPointSet(func, style);
    }
}

void Core::Graphics::Graph2D::drawCurves() {
    auto i=0;
    for(IN curveData : curves) {
        auto curve = curveData.curve;
        auto pointSet = curve.get()->renderToPointSet();
        auto points = pointSet.get()->getPoints();

        if(points.size()<2) continue;

        auto style = curveData.style;
        auto name  = curveData.name;

        nameLabelDraw(i, 1, style, name, this);

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


void Core::Graphics::Graph2D::setupOrtho() const {
    const Real deltaX = xMax-xMin;
    const Real deltaY = yMax-yMin;
    const Real xTraLeft   = 0;  // -deltaX*0.07;
    const Real xTraRight  = 0;  // +deltaX*0.02;
    const Real xTraTop    = 0;  // +deltaY*0.025;
    const Real xTraBottom = 0;  // -deltaY*0.025;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xMin+xTraLeft, xMax+xTraRight, (yMin+xTraBottom), (yMax+xTraTop), -1, 1);
}

void Core::Graphics::Graph2D::clearPointSets() { mPointSets.clear(); }

void Core::Graphics::Graph2D::clearCurves() { curves.clear(); }



auto Core::Graphics::Graph2D::getResolution() const -> Resolution        { return samples; }

auto Core::Graphics::Graph2D::setResolution(Resolution samples_) -> void { samples = samples_; }

RectR Core::Graphics::Graph2D::getLimits() const {
    return RectR(xMin, xMax, yMin, yMax);
}

auto Core::Graphics::Graph2D::setLimits(RectR lims) -> void {
    xMin = lims.xMin;
    xMax = lims.xMax;
    yMin = lims.yMin;
    yMax = lims.yMax;
}

void Core::Graphics::Graph2D::set_xMin(Real val) { xMin = val; }
void Core::Graphics::Graph2D::set_xMax(Real val) { xMax = val; }
void Core::Graphics::Graph2D::set_yMin(Real val) { yMin = val; }
void Core::Graphics::Graph2D::set_yMax(Real val) { yMax = val; }
Real Core::Graphics::Graph2D::get_xMin() const { return xMin; }
Real Core::Graphics::Graph2D::get_xMax() const { return xMax; }
Real Core::Graphics::Graph2D::get_yMin() const { return yMin; }
Real Core::Graphics::Graph2D::get_yMax() const { return yMax; }



bool Core::Graphics::Graph2D::notifyMouseButton(int button, int dir, int x, int y) {
    static auto time = Timer();

    if(button == 2){
        if(dir == 0) time.reset();
        else if(dir == 1 && time.getElTime_msec() < 200) savePopupOn = true;

        return true;
    }

    return EventListener::notifyMouseButton(button, dir, x, y);
}

bool Core::Graphics::Graph2D::notifyMouseMotion(int x, int y) {
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

bool Core::Graphics::Graph2D::notifyMouseWheel(int wheel, int direction, int x, int y) {
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

void Core::Graphics::Graph2D::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    auto vp = getViewport();
    writer.reshape(vp.w(), vp.h());
}











