//
// Created by joao on 29/05/23.
//

#include "Core/Tools/Log.h"

#include "PlottingWindow.h"
#include "PlotStyle.h"

#include <utility>
#include "3rdParty/ImGui.h"

#include "Utils/Printing.h"

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "3rdParty/glfreetype/TextRenderer.hpp"
#include "Core/Backend/BackendManager.h"
#include "PlotThemeManager.h"
#include "Artists/AxisArtist.h"

#define POPUP_ON_MOUSE_CALL false

#include "Graphics/Graph/Shapes/Shape.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"

namespace Slab {

    using Log = Core::Log;

    std::map<Str, Graphics::PlottingWindow *> Graphics::PlottingWindow::graphMap = {};
    Count Graphics::PlottingWindow::WindowCount = 0;

    using Mappy = Graphics::PlottingWindow::ContentMap;
    bool change_z_order(Mappy& mappy, Mappy::iterator &it, int z_order){
        if(z_order == it->first) return false;

        auto artist = it->second;
        it = mappy.erase(it);
        mappy.insert(std::make_pair(z_order, artist));

        return true;
    }
    bool z_order_up  (Mappy& mappy, Mappy::iterator it) { return change_z_order(mappy, it, it->first+1); }
    bool z_order_down(Mappy& mappy, Mappy::iterator it) { return change_z_order(mappy, it, it->first-1); }


#define Unique(label) \
    Str(Str(label) + "##" + ToStr(this->id)).c_str()

    Graphics::PlottingWindow::PlottingWindow(Real xMin, Real xMax, Real yMin, Real yMax, Str _title)
            : region{{xMin, xMax, yMin, yMax}}, title(std::move(_title)), axisArtist(), id(++WindowCount) {
        axisArtist.setLabel("Axis");
        artistXHair.setLabel("X-hair");
        labelsArtist.setLabel("Labels");

        if (title.empty()) title = Str("unnamed");
        Count n = 1;
        {
            Str uniqueTitle = title;
            while (PlottingWindow::graphMap.count(uniqueTitle))
                uniqueTitle = title + "(" + ToStr(++n) + ")";
            title = uniqueTitle;
        }
        PlottingWindow::graphMap[title] = this;

        Core::BackendManager::LoadModule(Core::RealTimeAnimation);

        Log::Note() << "Created PlottingWindow '" << title << "'" << Log::Flush;

        addArtist(Slab::Naked(axisArtist), 5);
        addArtist(Slab::Naked(labelsArtist), 5);
        addArtist(Slab::Naked(artistXHair), 6);

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

        Log::Note() << "PlottingWindow '" << this->title << "' added artist '" << pArtist->getLabel() << "'." << Log::Flush;
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

        if (autoReviewGraphRanges) reviewGraphRanges();

        setupOrtho();

        artistsDraw();

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
                for (auto it = content.begin(); it!=content.end(); ) {
                    IN artie = it->second;

                    bool increment_iterator = true;

                    auto label = artie->getLabel();
                    if (ImGui::ArrowButton((label + "##up").c_str(), ImGuiDir_Up)){
                        increment_iterator = !z_order_up(content, it);
                    }
                    ImGui::SameLine();
                    ImGui::Text("z=%i", it->first);
                    ImGui::SameLine();
                    if (ImGui::ArrowButton((label + "##down").c_str(), ImGuiDir_Down)) {
                        increment_iterator = !z_order_down(content, it);
                    }
                    ImGui::SameLine();

                    bool visible = artie->isVisible();
                    if (ImGui::Checkbox(Unique(artie->getLabel()), &visible)) {
                        artie->setVisibility(visible);
                    }

                    if(increment_iterator) ++it;
                }

                for (IN cont: content) {
                    IN artie = cont.second;

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
        OpenGL::Legacy::SetupOrtho(region.getRect());

        auto vp = getViewport();
        auto currStyle = PlotThemeManager::GetCurrent();
        currStyle->labelsWriter->reshape(vp.width(), vp.height());
        currStyle->ticksWriter->reshape(vp.width(), vp.height());
    }

    void Graphics::PlottingWindow::artistsDraw() {
        for (const auto &[priority, artist]: content)
            if (artist->isVisible() && !artist->draw(*this))
                Core::Log::Error() << "In PlottingWindow \"" << title
                << "\" while drawing Artist \"" << artist->getLabel() << "\"" << Core::Log::Flush;

    }

    Graphics::AxisArtist &
    Graphics::PlottingWindow::getAxisArtist() { return axisArtist; }

    void
    Graphics::PlottingWindow::setAutoReviewGraphRanges(bool autoReview) { autoReviewGraphRanges = autoReview; }

    void
    Graphics::PlottingWindow::toggleShowInterface() { showInterface = !showInterface; }

    void
    Graphics::PlottingWindow::requireLabelOverlay(const Str& label, const Pointer<Graphics::PlotStyle>& style) const {
        const_cast<LabelsArtist*>(&labelsArtist)->add(label, style);
    }

}