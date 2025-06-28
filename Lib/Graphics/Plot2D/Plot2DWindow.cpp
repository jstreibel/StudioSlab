//
// Created by joao on 29/05/23.
//

#include "Core/Tools/Log.h"

#include "Graphics/SlabGraphics.h"

#include "Plot2DWindow.h"
#include "PlotStyle.h"

#include <utility>
#include <sys/stat.h>

#include "3rdParty/ImGui.h"

#include "Utils/Printing.h"

#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"

#include "PlotThemeManager.h"
#include "Artists/AxisArtist.h"

#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Core/SlabCore.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "StudioSlab.h"

namespace Slab::Graphics {

    using Log = Core::Log;

    std::map<Str, Plot2DWindow *> Plot2DWindow::graphMap = {};
    CountType Plot2DWindow::WindowCount = 0;

    using Mappy = Plot2DWindow::ContentMap;
    bool change_z_order(Mappy& mappy, Mappy::iterator &it, int z_order){
        if(z_order == it->first) return false;

        auto artist = it->second;
        it = mappy.erase(it);
        mappy.insert(std::make_pair(z_order, artist));

        return true;
    }
    bool z_order_up  (Mappy& mappy, Mappy::iterator it) { return change_z_order(mappy, it, it->first+1); }
    bool z_order_down(Mappy& mappy, Mappy::iterator it) { return change_z_order(mappy, it, it->first-1); }

    Plot2DWindow::Plot2DWindow(DevFloat xMin, DevFloat xMax, DevFloat yMin, DevFloat yMax, Str _title)
    : id(++WindowCount)
    , region{{xMin, xMax, yMin, yMax}}
    , title(std::move(_title))
    , axisArtist()
    {
        // Instantiate our dedicated Plot themes manager
        PlotThemeManager::GetInstance();

        if(this->gui_context == nullptr) {
            gui_context_is_local = true;
            auto &gui_module = Slab::GetModule<ImGuiModule>("ImGui");
            gui_context = DynamicPointerCast<SlabImGuiContext>(gui_module.createContext(parent_system_window));
            addResponder(gui_context);
        }

        axisArtist.setLabel("Axis");
        artistXHair.setLabel("X-hair");
        labelsArtist.setLabel("Labels");

        if (title.empty()) title = Str("unnamed");
        CountType n = 1;
        {
            Str uniqueTitle = title;
            while (Plot2DWindow::graphMap.count(uniqueTitle))
                uniqueTitle = title + "(" + ToStr(++n) + ")";
            title = uniqueTitle;
        }
        Plot2DWindow::graphMap[title] = this;

        Core::LoadModule("ImGui");
        Core::LoadModule("RealTimeAnimation");

        Log::Note() << "Created PlottingWindow '" << title << "'" << Log::Flush;

        addArtist(Slab::Naked(axisArtist), 5);
        addArtist(Slab::Naked(labelsArtist), 5);
        addArtist(Slab::Naked(artistXHair), 6);
        addArtist(Slab::Naked(bgArtist), -10);

    }

    Plot2DWindow::Plot2DWindow(Str title)
            : Plot2DWindow(-1, 1, -1, 1, std::move(title)) {    }

    void Plot2DWindow::addArtist(const Artist_ptr &pArtist, zOrder_t zOrder) {
        if (pArtist == nullptr) {
            Log::Error() << __PRETTY_FUNCTION__ << " trying to add "
                         << Log::FGBlue << "nullptr" << Log::ResetFormatting << " artist.";

            return;
        }

        content.emplace(zOrder, pArtist);

        Log::Note() << "PlottingWindow '" << this->title << "' added artist '" << pArtist->getLabel() << "'." << Log::Flush;
    }

    bool Plot2DWindow::removeArtist(const Graphics::Artist_ptr &pArtist) {
        auto haveErased = false;

        for (auto item = content.begin(); item != content.end();) {
            if (item->second == pArtist) {
                item = content.erase(item);
                haveErased = true;
            } else ++item;
        }

        return haveErased;
    }

    void Plot2DWindow::draw() {
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + "; '" + title + "'");

        SlabWindow::draw();

        if (autoReviewGraphRanges) reviewGraphRanges();

        artistsDraw();

        drawGUI();
    }

    void Graphics::Plot2DWindow::drawGUI() {
        static bool bypass_gui = false; // When outputting to png, the whole draw for this window (including this
        if (bypass_gui) { return; }     // function) is performed on top of the current draw. We thus bypass the gui
                                        // draw to not get ImGui santity check "Forgot to call Render() or EndFrame()
                                        // at the end of the previous frame?"

        auto draw_call = [this](){
            auto popupName = AddUniqueIdToString(title + Str(" window popup"));

            if (popupOn && !POPUP_ON_MOUSE_CALL) {

                ImGui::OpenPopup(AddUniqueIdToString(popupName).c_str());
                popupOn = false;
            }

            if (ImGui::BeginPopup(AddUniqueIdToString(popupName).c_str())) {
                if(ImGui::MenuItem("Auto adjust", nullptr, autoReviewGraphRanges)) {
                    autoReviewGraphRanges = !autoReviewGraphRanges;
                }
                else if (ImGui::MenuItem(AddUniqueIdToString("Show interface").c_str(), nullptr, showInterface)) {
                    showInterface = !showInterface;
                } else if (ImGui::MenuItem(AddUniqueIdToString("Save graph").c_str())) {

                    auto w = Printing::getTotalHorizontalDots(.5);
                    auto h = w * .5;
                    auto fileName = title + " " +
                                    Core::FCommandLineInterfaceManager::getInstance().renderParametersToString({"N", "L"}) +
                                    ".png";
                    bypass_gui = true;
                    OpenGL::outputToPNG(this, fileName, w, (int) h);
                    bypass_gui = false;
                }

                ImGui::EndPopup();
            }

            if (showInterface || !gui_context_is_local)
            {
                bool began;

                if(gui_context_is_local) {
                    auto vp = getViewport();
                    // auto sh = Slab::Graphics::GetGraphicsBackend()->getScreenHeight();
                    // ImGui::SetNextWindowPos({(float)vp.xMin, (float)(sh-(vp.yMin+vp.height()))}, ImGuiCond_Appearing);
                    ImGui::SetNextWindowPos({(float)vp.xMin, (float)(vp.yMin)}, ImGuiCond_Always);
                    ImGui::SetNextWindowSize({.0f, (float)vp.height()}, ImGuiCond_Always);

                    ImGui::SetNextWindowBgAlpha(0.65);
                    began = ImGui::Begin(title.c_str(), &showInterface, ImGuiWindowFlags_NoFocusOnAppearing
                    | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
                } else {
                    bool closable = false;

                    ImGui::Begin("Stats", &closable,
                                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

                    ImGuiTreeNodeFlags flags = 0;
                    if(isMouseIn()) {
                        flags = ImGuiTreeNodeFlags_Selected;
                    }
                    began = ImGui::CollapsingHeader(AddUniqueIdToString(title).c_str(), flags);
                }

                if (began) {
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
                        if (ImGui::Checkbox((AddUniqueIdToString(artie->getLabel())+"_checkbox").c_str() , &visible)) {
                            artie->setVisibility(visible);
                        }

                        if(increment_iterator) ++it;
                    }

                    if(!gui_context_is_local) {
                        auto avail_region = ImGui::GetContentRegionAvail();
                        ImGui::BeginChild(AddUniqueIdToString(title + " :)").c_str(), {avail_region.x, 0},
                                          ImGuiChildFlags_Border
                                          | ImGuiChildFlags_AutoResizeY);
                    }

                    for (IN cont: content) {
                        IN artie = cont.second;

                        if (artie->isVisible() && artie->hasGUI()) {
                            if (ImGui::CollapsingHeader((AddUniqueIdToString(artie->getLabel())).c_str()))
                                artie->drawGUI();
                        }
                    }

                    if(!gui_context_is_local) ImGui::EndChild();
                }

                ImGui::End();
            }};

        if(gui_context_is_local) {
            gui_context->NewFrame();
            draw_call();
            gui_context->Render();
        } else {
            gui_context->AddDrawCall(draw_call);
        }
    }

    void Graphics::Plot2DWindow::setupOrtho() const {

        OpenGL::Legacy::ResetModelview();

        OpenGL::Legacy::SetupOrtho(region.getRect());

        auto vp = getViewport();
        auto currStyle = PlotThemeManager::GetCurrent();
        currStyle->labelsWriter->reshape(vp.width(), vp.height());
        currStyle->ticksWriter->reshape(vp.width(), vp.height());
    }

    void Graphics::Plot2DWindow::artistsDraw() {
        setupOrtho();

        for (const auto &[priority, artist]: content)
            if (artist->isVisible() && !artist->draw(*this))
                Core::Log::Error() << "In PlottingWindow \"" << title
                << "\" while drawing Artist \"" << artist->getLabel() << "\"" << Core::Log::Flush;

    }

    Graphics::AxisArtist &
    Graphics::Plot2DWindow::getAxisArtist() { return axisArtist; }

    void
    Graphics::Plot2DWindow::setAutoReviewGraphRanges(bool autoReview) { autoReviewGraphRanges = autoReview; }

    void
    Graphics::Plot2DWindow::toggleShowInterface() { showInterface = !showInterface; }

    void
    Graphics::Plot2DWindow::requireLabelOverlay(const Str& label, const Pointer<Graphics::PlotStyle>& style) const {
        const_cast<LabelsArtist*>(&labelsArtist)->add(label, style);
    }

    void Graphics::Plot2DWindow::tieRegion_xMaxMin(const Graphics::Plot2DWindow &other) {
        region.setReference_xMin(other.getRegion().getReference_xMin());
        region.setReference_xMax(other.getRegion().getReference_xMax());
    }

    void Graphics::Plot2DWindow::tieRegion_yMaxMin(const Graphics::Plot2DWindow &other) {
        region.setReference_yMin(other.getRegion().getReference_yMin());
        region.setReference_yMax(other.getRegion().getReference_yMax());
    }

    void Graphics::Plot2DWindow::removeArtists(const Vector<Graphics::Artist_ptr> &artists) {
        for(const auto& artist : artists) removeArtist(artist);
    }

}