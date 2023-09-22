//
// Created by joao on 29/05/23.
//

#include "Graph.h"

#include <utility>
#include "imgui.h"

#include "Utils/Printing.h"
#include "Core/Tools/Log.h"
#include "Core/Tools/Animator.h"

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "3rdParty/glfreetype/TextRenderer.hpp"
#include "Core/Backend/BackendManager.h"

#define POPUP_ON_MOUSE_CALL false
#define ANIMATION_TIME_SECONDS 0.2

std::map<Str, Core::Graphics::Graph2D*> Core::Graphics::Graph2D::graphMap = {};

Core::Graphics::Graph2D::Graph2D(Real xMin, Real xMax, Real yMin, Real yMax, Str _title, int samples)
: region{xMin, xMax, yMin, yMax}
, title(std::move(_title))
, samples(samples)
{
    this->backgroundColor = Styles::GetCurrent()->graphBackground;

    if(title.empty()) title = Str("unnamed");
    Count n=1;
    while(Graph2D::graphMap.count(title))
        title += "(" + ToStr(++n) + ")";
    Graph2D::graphMap[title] = this;

    Log::Info() << "Created Graph2D '" << title << "'" << Log::Flush;
}

Core::Graphics::Graph2D::Graph2D(Str title, bool autoReviewGraphLimits)
: Graph2D(-1,1,-1,1,std::move(title))
{
    autoReviewGraphRanges = autoReviewGraphLimits;
}

void Core::Graphics::Graph2D::draw() {
    OpenGLUtils::checkGLErrors(Str(__PRETTY_FUNCTION__) + "; '" + title + "'");
    Window::draw();

    if (autoReviewGraphRanges) reviewGraphRanges();
    setupOrtho();

    drawAxes();

    labelingHelper.setTotalItems(countDisplayItems());

    drawPointSets();
    drawCurves();

    drawGUI();

    drawXHair();
}


void Core::Graphics::Graph2D::drawGUI() {
    auto popupName = title + Str(" window popup");

    if(savePopupOn && !POPUP_ON_MOUSE_CALL) {
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
    for(auto &ptSet : mPointSets){
        auto &func = *ptSet.data;
        auto style = ptSet.plotStyle;
        auto label = ptSet.name;

        if(!label.empty())
            nameLabelDraw(style, label);

        Core::Graphics::Graph2D::renderPointSet(func, style);
    }
}

void Core::Graphics::Graph2D::drawCurves() {
    for(IN curveData : curves) {
        auto curve = curveData.curve;
        auto pointSet = curve.get()->renderToPointSet();
        auto points = pointSet.get()->getPoints();

        if(points.size()<2) continue;

        auto style = curveData.style;
        auto name  = curveData.name;

        OpenGL::Shader::remove();

        nameLabelDraw(style, name);

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


void
Core::Graphics::Graph2D::addPointSet(Spaces::PointSet::Ptr pointSet,
                                     Styles::PlotStyle style,
                                     Str setName,
                                     bool affectsGraphRanges) {
    auto metaData = PointSetMetadata{std::move(pointSet), style, setName, affectsGraphRanges};
    mPointSets.emplace_back(metaData);
}

void
Core::Graphics::Graph2D::addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, Str name) {
    CurveMetadata curveMetadata = {std::move(curve), style, std::move(name)};
    curves.emplace_back(curveMetadata);
}

void
Core::Graphics::Graph2D::renderPointSet(const Spaces::PointSet &pSet,
                                        Styles::PlotStyle style) noexcept {
    auto pts = pSet.getPoints();

    OpenGL::Shader::remove();

    if(style.filled && !(style.primitive==Styles::Point || style.primitive==Styles::Lines))
    {
        const auto color = style.fillColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glBegin(GL_QUADS);
        {
            auto iMax = (long)pts.size()-1;
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

        if(style.primitive != Styles::SolidLine
        && style.primitive != Styles::VerticalLines
        && style.primitive != Styles::Lines){
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else glEnable(GL_LINE_SMOOTH);

        auto primitive = GL_LINE_STRIP;
        if(style.primitive==Styles::Point || style.primitive==Styles::VerticalLines){
            fix ptSizeFactor = style.primitive==Styles::VerticalLines ? 5.0 : 1.0;

            primitive = GL_POINTS;
            glEnable(GL_POINT_SMOOTH);
            glPointSize(style.thickness * ptSizeFactor);

            glEnable(GL_LINE_SMOOTH);
            glLineWidth(style.thickness);
        } else if(style.primitive==Styles::Lines) {
            primitive = GL_LINES;
        }

        glBegin(primitive);
        {
            for(auto p : pts)
                glVertex2d(p.x, p.y);

        }
        glEnd();

        if(style.primitive==Styles::VerticalLines){
            glBegin(GL_LINES);
            {
                for(auto p : pts) {
                    glVertex2d(p.x, 0.0);
                    glVertex2d(p.x, p.y);
                }
            }
            glEnd();
        }

    }
}

void Core::Graphics::Graph2D::setupOrtho() const {
    const Real deltaX = region.width();
    const Real deltaY = region.height();
    const Real xTraLeft   = 0;  // -deltaX*0.07;
    const Real xTraRight  = 0;  // +deltaX*0.02;
    const Real xTraTop    = 0;  // +deltaY*0.025;
    const Real xTraBottom = 0;  // -deltaY*0.025;

    OpenGL::Shader::remove();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(region.xMin+xTraLeft, region.xMax+xTraRight, (region.yMin+xTraBottom), (region.yMax+xTraTop), -1, 1);

    auto vp = getViewport();
    Styles::GetCurrent()->labelsWriter->reshape(vp.width(), vp.height());
    Styles::GetCurrent()->ticksWriter->reshape(vp.width(), vp.height());
}

void Core::Graphics::Graph2D::clearPointSets() { mPointSets.clear(); }
void Core::Graphics::Graph2D::clearCurves() { curves.clear(); }


auto Core::Graphics::Graph2D::getResolution() const -> Resolution        { return samples; }
auto Core::Graphics::Graph2D::setResolution(Resolution samples_) -> void { samples = samples_; }
const RectR& Core::Graphics::Graph2D::getRegion() const { return region; }
auto Core::Graphics::Graph2D::setLimits(RectR lims) -> void { region = lims; }

void Core::Graphics::Graph2D::set_xMin(Real val) { Animator::Add(region.xMin, val, animationTimeSeconds); }
void Core::Graphics::Graph2D::set_xMax(Real val) { Animator::Add(region.xMax, val, animationTimeSeconds); }
void Core::Graphics::Graph2D::set_yMin(Real val) { Animator::Add(region.yMin, val, animationTimeSeconds); }
void Core::Graphics::Graph2D::set_yMax(Real val) { Animator::Add(region.yMax, val, animationTimeSeconds); }
Real Core::Graphics::Graph2D::get_xMin() const { return region.xMin; }
Real Core::Graphics::Graph2D::get_xMax() const { return region.xMax; }
Real Core::Graphics::Graph2D::get_yMin() const { return region.yMin; }
Real Core::Graphics::Graph2D::get_yMax() const { return region.yMax; }

bool Core::Graphics::Graph2D::notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) {
    static auto time = Timer();

    if(button == Core::MouseButton_RIGHT){
        if(state == Press) time.reset();
        else if(state == Release && time.getElTime_msec() < 200){
            savePopupOn = true;

            auto popupName = Str("win_") + title + Str("_popup");
            if(POPUP_ON_MOUSE_CALL) {
                Log::Info() << "Popup (on mouse call) '" << popupName << "' is on" << Log::Flush;
                ImGui::OpenPopup(popupName.c_str());
                savePopupOn = false;
            }
        }

        return true;
    }

    return GUIEventListener::notifyMouseButton(button, state, keys);
}

bool Core::Graphics::Graph2D::notifyMouseMotion(int x, int y) {
    auto elRet = GUIEventListener::notifyMouseMotion(x, y);

    auto& mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

    if(mouseState.leftPressed)
    {
        const Real dxClampd = - mouseState.dx / (Real)getw();
        const Real dyClampd = mouseState.dy / (Real)geth();
        const Real wGraph = region.width();
        const Real hGraph = region.height();
        const Real dxGraph = wGraph * dxClampd;
        const Real dyGraph = hGraph * dyClampd;

        region.xMin += dxGraph;
        region.xMax += dxGraph;
        region.yMin += dyGraph;
        region.yMax += dyGraph;
    }
    if(mouseState.centerPressed)
    {
        constexpr const Real factor = 0.01;
        const Real dx = 1-factor*mouseState.dx;
        const Real dy = 1+factor*mouseState.dy;

        const Real x0 = region.xCenter();
        const Real y0 = region.yCenter();
        const Real hw = .5 * region.width() *dx;
        const Real hh = .5 * region.height()*dy;

        region = {
            x0 - hw,
            x0 + hw,
            y0 - hh,
            y0 + hh
        };
    }

    return elRet;
}

bool Core::Graphics::Graph2D::notifyMouseWheel(double dx, double dy) {
    GUIEventListener::notifyMouseWheel(dx, dy);

    constexpr const Real factor = 1.2;
    const Real d = pow(factor, -dx);

    static auto targetRegion = region;

    if(!Animator::Contains(region.xMin)
       && !Animator::Contains(region.xMax)
       && !Animator::Contains(region.yMin)
       && !Animator::Contains(region.yMax)) {
        targetRegion = region;
    }

    if(Core::BackendManager::GetGUIBackend().getMouseState().rightPressed) {

        const Real x0 = targetRegion.xCenter();
        const Real hw = .5*targetRegion.width() * d;

        targetRegion.xMin = x0-hw;
        targetRegion.xMax = x0+hw;

        set_xMin(targetRegion.xMin);
        set_xMax(targetRegion.xMax);

    } else {
        const Real y0 = targetRegion.yCenter();
        const Real hh = .5 * targetRegion.height() * d;

        targetRegion.yMin = y0-hh;
        targetRegion.yMax = y0+hh;

        set_yMin(targetRegion.yMin);
        set_yMax(targetRegion.yMax);
    }

    return true;
}



void Core::Graphics::Graph2D::notifyReshape(int newWinW, int newWinH) { Window::notifyReshape(newWinW, newWinH); }

auto Core::Graphics::Graph2D::countDisplayItems() const -> Count { return mPointSets.size() + curves.size(); }

void Core::Graphics::Graph2D::setAnimationTime(Real value) { animationTimeSeconds = value; }

Real Core::Graphics::Graph2D::getAnimationTime() const { return animationTimeSeconds; }

void Core::Graphics::Graph2D::setHorizontalUnit(const Unit &hUnit) { baseHorizontalUnit = hUnit; }

void Core::Graphics::Graph2D::setVerticalUnit(const Unit &hUnit)   { baseVerticalUnit   = hUnit; }












