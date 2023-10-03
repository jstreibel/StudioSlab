//
// Created by joao on 29/05/23.
//

#include "Graph.h"
#include "Styles.h"

#include <utility>
#include "imgui.h"

#include "Utils/Printing.h"
#include "Core/Tools/Log.h"

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "3rdParty/glfreetype/TextRenderer.hpp"
#include "Core/Backend/BackendManager.h"
#include "StylesManager.h"
#include "Artists/AxisArtist.h"

#define POPUP_ON_MOUSE_CALL false

std::map<Str, Graphics::Graph2D *> Graphics::Graph2D::graphMap = {};


Graphics::Graph2D::Graph2D(Real xMin, Real xMax, Real yMin, Real yMax, Str _title, int samples)
: region{xMin, xMax, yMin, yMax}
, title(std::move(_title))
, samples(samples)
, axisArtist()
{
    if (title.empty()) title = Str("unnamed");
    Count n = 1;
    while (Graph2D::graphMap.count(title))
        title += "(" + ToStr(++n) + ")";
    Graph2D::graphMap[title] = this;

    Core::BackendManager::LoadModule(Core::RealTimeAnimation);

    Log::Info() << "Created Graph2D '" << title << "'" << Log::Flush;

    addArtist(DummyPtr(axisArtist), 100);
    addArtist(DummyPtr(artistXHair), 90);
}

Graphics::Graph2D::Graph2D(Str title, bool autoReviewGraphLimits)
        : Graph2D(-1, 1, -1, 1, std::move(title)) {
    autoReviewGraphRanges = autoReviewGraphLimits;
}

void Graphics::Graph2D::addArtist(const Artist::Ptr& pArtist, zOrder_t zOrder) {
    content.emplace(zOrder, pArtist);
}

void Graphics::Graph2D::draw() {
    OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + "; '" + title + "'");
    Window::setClearColor(Math::StylesManager::GetCurrent()->graphBackground);
    Window::draw();

    artistXHair.setVisibility(isMouseIn());

    if (autoReviewGraphRanges) reviewGraphRanges();
    setupOrtho();

    artistsDraw();

    labelingHelper.setTotalItems(countDisplayItems());

    drawPointSets();
    drawCurves();

    drawGUI();
}

void Graphics::Graph2D::drawGUI() {
    auto popupName = title + Str(" window popup");

    if (savePopupOn && !POPUP_ON_MOUSE_CALL) {
        ImGui::OpenPopup(popupName.c_str());
        savePopupOn = false;
    }

    if (ImGui::BeginPopup(popupName.c_str())) {
        if (ImGui::MenuItem("Save graph")) {
            auto w = Printing::getTotalHorizontalDots(.5);
            auto h = w * .5;
            auto fileName = title + " " +
                            InterfaceManager::getInstance().renderParametersToString({"N", "L"}) +
                            ".png";
            OpenGL::outputToPNG(this, fileName, w, (int) h);
        }
        ImGui::EndPopup();
    }
}

void Graphics::Graph2D::drawPointSets() {

    for (auto &ptSet: mPointSets) {
        auto &func = *ptSet.data;
        auto style = ptSet.plotStyle;
        auto label = ptSet.name;

        if (!label.empty())
            nameLabelDraw(style, label);

        Graphics::Graph2D::renderPointSet(func, style);
    }
}

void Graphics::Graph2D::drawCurves() {
    for (IN curveData: curves) {
        auto curve = curveData.curve;
        auto pointSet = curve.get()->renderToPointSet();
        auto points = pointSet.get()->getPoints();

        if (points.size() < 2) continue;

        auto style = curveData.style;
        auto name = curveData.name;

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
        if (style.primitive == Styles::Point) primitive = GL_POINTS;

        glBegin(primitive);
        {
            for (const auto &p: points)
                glVertex2d(p.x, p.y);
        }
        glEnd();
    }
}

void
Graphics::Graph2D::addPointSet(Spaces::PointSet::Ptr pointSet,
                               Styles::PlotStyle style,
                               Str setName,
                               bool affectsGraphRanges) {
    auto metaData = PointSetMetadata{std::move(pointSet), style, std::move(setName),
                                     affectsGraphRanges};
    mPointSets.push_back(metaData);
}

void Graphics::Graph2D::removePointSet(const Spaces::PointSet::Ptr &pointSet) {
    mPointSets.remove_if([&pointSet](const PointSetMetadata &ptSetMetadata) {
        return ptSetMetadata.data == pointSet;
    });
}

void Graphics::Graph2D::removePointSet(const Str &name) {
    mPointSets.remove_if([&name](const PointSetMetadata &ptSetMetadata) {
        return ptSetMetadata.name == name;
    });
}

void
Graphics::Graph2D::addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, Str name) {
    CurveMetadata curveMetadata = {std::move(curve), style, std::move(name)};
    curves.emplace_back(curveMetadata);
}

void
Graphics::Graph2D::renderPointSet(const Spaces::PointSet &pSet,
                                  Styles::PlotStyle style) noexcept {
    auto pts = pSet.getPoints();

    OpenGL::Shader::remove();

    if (style.filled && !(style.primitive == Styles::Point || style.primitive == Styles::Lines)) {
        const auto color = style.fillColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glBegin(GL_QUADS);
        {
            auto iMax = (long) pts.size() - 1;
            for (auto i = 0; i < iMax; ++i) {
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

        if (style.primitive != Styles::SolidLine
            && style.primitive != Styles::VerticalLines
            && style.primitive != Styles::Lines) {
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else glEnable(GL_LINE_SMOOTH);

        auto primitive = GL_LINE_STRIP;
        if (style.primitive == Styles::Point || style.primitive == Styles::VerticalLines) {
            fix ptSizeFactor = style.primitive == Styles::VerticalLines ? 5.0 : 1.0;

            primitive = GL_POINTS;
            glEnable(GL_POINT_SMOOTH);
            glPointSize(style.thickness * ptSizeFactor);

            glEnable(GL_LINE_SMOOTH);
            glLineWidth(style.thickness);
        } else if (style.primitive == Styles::Lines) {
            primitive = GL_LINES;
        }

        glBegin(primitive);
        {
            for (auto p: pts)
                glVertex2d(p.x, p.y);

        }
        glEnd();

        if (style.primitive == Styles::VerticalLines) {
            glBegin(GL_LINES);
            {
                for (auto p: pts) {
                    glVertex2d(p.x, 0.0);
                    glVertex2d(p.x, p.y);
                }
            }
            glEnd();
        }

    }
}

void Graphics::Graph2D::setupOrtho() const {
    // const Real deltaX = region.width();
    // const Real deltaY = region.height();
    // const Real xTraLeft   = 0-deltaX*0.07;
    // const Real xTraRight  = 0+deltaX*0.02;
    // const Real xTraTop    = 0+deltaY*0.025;
    // const Real xTraBottom = 0-deltaY*0.025;

    OpenGL::Shader::remove();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(region.xMin+xTraLeft, region.xMax+xTraRight, (region.yMin+xTraBottom), (region.yMax+xTraTop), -1, 1);
    glOrtho(region.xMin, region.xMax, region.yMin, region.yMax, -1, 1);

    auto vp = getViewport();
    auto currStyle = Math::StylesManager::GetCurrent();
    currStyle->labelsWriter->reshape(vp.width(), vp.height());
    currStyle->ticksWriter->reshape(vp.width(), vp.height());
}

void Graphics::Graph2D::clearPointSets() { mPointSets.clear(); }

void Graphics::Graph2D::clearCurves() { curves.clear(); }

auto Graphics::Graph2D::countDisplayItems() const -> Count {
    return mPointSets.size() + curves.size();
}

void Graphics::Graph2D::artistsDraw() {
    for (const auto & [priority, artist] : content) artist->draw(*this);
}
