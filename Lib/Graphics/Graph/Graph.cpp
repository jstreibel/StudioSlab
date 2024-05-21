//
// Created by joao on 29/05/23.
//

#include "Graph.h"
#include "PlotStyle.h"

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
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

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

    addArtist(Slab::DummyPointer(axisArtist), 100);
    addArtist(Slab::DummyPointer(artistXHair), 10000);
}

Graphics::Graph2D::Graph2D(Str title, bool autoReviewGraphLimits)
        : Graph2D(-1, 1, -1, 1, std::move(title)) {
    autoReviewGraphRanges = autoReviewGraphLimits;
}

void Graphics::Graph2D::addArtist(const Artist_ptr& pArtist, zOrder_t zOrder) {
    content.emplace(zOrder, pArtist);
}

bool Graphics::Graph2D::removeArtist(const Graphics::Artist_ptr &pArtist) {
    auto haveErased = false;

    for(auto item = content.begin(); item!=content.end();) {
        if(item->second==pArtist){
            item=content.erase(item);
            haveErased = true;
        }
        else ++item;
    }

    return haveErased;
}

void Graphics::Graph2D::draw() {
    OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + "; '" + title + "'");
    Window::setBGColor(StylesManager::GetCurrent()->graphBackground);
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

        Graphics::OpenGL::Legacy::RenderPointSet(func, style);
    }
}

void Graphics::Graph2D::drawCurves() {
    for (IN curveData: curves) {
        auto curve = curveData.curve;
        auto pointSet = curve.get()->renderToPointSet();

        auto style = curveData.style;
        auto name = curveData.name;

        nameLabelDraw(style, name);

        Graphics::OpenGL::Legacy::RenderPointSet(*pointSet, style);
    }
}

void
Graphics::Graph2D::addPointSet(Math::PointSet_ptr pointSet,
                               PlotStyle style,
                               Str setName,
                               bool affectsGraphRanges) {
    auto metaData = PointSetMetadata{std::move(pointSet), style, std::move(setName),
                                     affectsGraphRanges};
    mPointSets.push_back(metaData);
}

void Graphics::Graph2D::removePointSet(const Math::PointSet_ptr &pointSet) {
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
Graphics::Graph2D::addCurve(RtoR2::ParametricCurve::Ptr curve, PlotStyle style, Str name) {
    CurveMetadata curveMetadata = {std::move(curve), style, std::move(name)};
    curves.emplace_back(curveMetadata);
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
    auto currStyle = StylesManager::GetCurrent();
    currStyle->labelsWriter->reshape(vp.width(), vp.height());
    currStyle->ticksWriter->reshape(vp.width(), vp.height());
}

void Graphics::Graph2D::clearPointSets() { mPointSets.clear(); }

void Graphics::Graph2D::clearCurves() { curves.clear(); }

auto Graphics::Graph2D::countDisplayItems() const -> Count {
    return mPointSets.size() + curves.size();
}

void Graphics::Graph2D::artistsDraw() {
    for (const auto & [priority, artist] : content)
        artist->draw(*this);
}

Graphics::AxisArtist &Graphics::Graph2D::getAxisArtist() {
    return axisArtist;
}


