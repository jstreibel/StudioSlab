//
// Created by joao on 29/05/23.
//

#include "Core/Tools/Log.h"

#include "PlottingWindow.h"
#include "PlotStyle.h"

#include <utility>
#include "imgui.h"

#include "Utils/Printing.h"

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "3rdParty/glfreetype/TextRenderer.hpp"
#include "Core/Backend/BackendManager.h"
#include "PlotThemeManager.h"
#include "Artists/AxisArtist.h"

#define POPUP_ON_MOUSE_CALL false


namespace Slab {

    using Log = Core::Log;

    std::map<Str, Graphics::PlottingWindow *> Graphics::PlottingWindow::graphMap = {};
    Count Graphics::PlottingWindow::WindowCount = 0;

#define Unique(label) \
    Str(Str(label) + "##" + ToStr(this->id)).c_str()

    Graphics::PlottingWindow::PlottingWindow(Real xMin, Real xMax, Real yMin, Real yMax, Str _title)
            : region{xMin, xMax, yMin, yMax}, title(std::move(_title)), axisArtist(), id(++WindowCount) {
        axisArtist.setLabel("Axis");
        artistXHair.setLabel("X-hair");

        if (title.empty()) title = Str("unnamed");
        Count n = 1;
        while (PlottingWindow::graphMap.count(title))
            title += "(" + ToStr(++n) + ")";
        PlottingWindow::graphMap[title] = this;

        Core::BackendManager::LoadModule(Core::RealTimeAnimation);

        Log::Info() << "Created PlottingWindow '" << title << "'" << Log::Flush;

        addArtist(Slab::DummyPointer(axisArtist), 100);
        addArtist(Slab::DummyPointer(artistXHair), 10000);
    }

    Graphics::PlottingWindow::PlottingWindow(Str title, bool autoReviewGraphLimits)
            : PlottingWindow(-1, 1, -1, 1, std::move(title)) {
        autoReviewGraphRanges = autoReviewGraphLimits;
    }

    void Graphics::PlottingWindow::addArtist(const Artist_ptr &pArtist, zOrder_t zOrder) {
        if (pArtist == nullptr) {
            Log::Error() << __PRETTY_FUNCTION__ << " trying to add "
                         << Log::FGBlue << "nullptr" << Log::ResetFormatting << " artist.";

            return;
        }

        content.emplace(zOrder, pArtist);
    }

    bool Graphics::PlottingWindow::removeArtist(const Graphics::Artist_ptr &pArtist) {
        auto haveErased = false;

        for (auto item = content.begin(); item != content.end();) {
            if (item->second == pArtist) {
                item = content.erase(item);
                haveErased = true;
            } else ++item;
        }

        return haveErased;
    }

    void Graphics::PlottingWindow::draw() {
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + "; '" + title + "'");
        Window::setBGColor(PlotThemeManager::GetCurrent()->graphBackground);
        Window::draw();

        artistXHair.setVisibility(isMouseIn());

        if (autoReviewGraphRanges) reviewGraphRanges();

        setupOrtho();

        artistsDraw();

        labelingHelper.setTotalItems(countDisplayItems());

        drawGUI();
    }

    void Graphics::PlottingWindow::drawGUI() {
        auto popupName = title + Str(" window popup");

        if (popupOn && !POPUP_ON_MOUSE_CALL) {
            ImGui::OpenPopup(Unique(popupName));
            popupOn = false;
        }

        if (ImGui::BeginPopup(Unique(popupName))) {
            if(ImGui::MenuItem("Auto adjust", NULL, autoReviewGraphRanges)) {
                autoReviewGraphRanges = !autoReviewGraphRanges;
            }
            else if (ImGui::MenuItem(Unique("Show interface"), NULL, showInterface)) {
                showInterface = !showInterface;
            } else if (ImGui::MenuItem(Unique("Save graph"))) {
                auto w = Printing::getTotalHorizontalDots(.5);
                auto h = w * .5;
                auto fileName = title + " " +
                                Core::InterfaceManager::getInstance().renderParametersToString({"N", "L"}) +
                                ".png";
                OpenGL::outputToPNG(this, fileName, w, (int) h);
            }

            ImGui::EndPopup();
        }

        if (showInterface) {
            auto vp = getViewport();
            auto sh = Core::BackendManager::GetGUIBackend().getScreenHeight();

            ImGui::SetNextWindowPos({(float)vp.xMin, (float)(sh-(vp.yMin+vp.height()))}, ImGuiCond_Appearing);
            ImGui::SetNextWindowSize({(float)vp.width()*.20f, (float)vp.height()}, ImGuiCond_Appearing);

            if (ImGui::Begin(title.c_str(), &showInterface)) {

                for (IN cont: content) {
                    IN artie = cont.second;

                    if (artie.get() == &artistXHair) continue;

                    bool visible = artie->isVisible();

                    if (ImGui::Checkbox(Unique(artie->getLabel()), &visible))
                        artie->setVisibility(visible);
                }

                for (IN cont: content) {
                    IN artie = cont.second;

                    if (artie.get() == &artistXHair) continue;

                    if (artie->isVisible() && artie->hasGUI()) {
                        if (ImGui::CollapsingHeader(Unique(artie->getLabel())))
                            artie->drawGUI();
                    }
                }
            }

            ImGui::End();
        }
    }

    void Graphics::PlottingWindow::setupOrtho() const {
        OpenGL::Shader::remove();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(region.xMin, region.xMax, region.yMin, region.yMax, -1, 1);

        auto vp = getViewport();
        auto currStyle = PlotThemeManager::GetCurrent();
        currStyle->labelsWriter->reshape(vp.width(), vp.height());
        currStyle->ticksWriter->reshape(vp.width(), vp.height());
    }

    auto Graphics::PlottingWindow::countDisplayItems() const -> Count {
        Count n = 0;
        for (const auto &a: content)
            if (a.second->wantsLegend()) ++n;

        return n;
    }

    void Graphics::PlottingWindow::artistsDraw() {
        for (const auto &[priority, artist]: content)
            if (artist->isVisible() && !artist->draw(*this))
                Core::Log::Error() << "In PlottingWindow \"" << title
                << "\" while drawing Artist \"" << artist->getLabel() << "\"" << Core::Log::Flush;

    }

    Graphics::AxisArtist &Graphics::PlottingWindow::getAxisArtist() {
        return axisArtist;
    }

    void Graphics::PlottingWindow::setAutoReviewGraphRanges(bool autoReview) {
        autoReviewGraphRanges = autoReview;
    }


}