//
// Created by joao on 29/05/23.
//

#include "Core/Tools/Log.h"

#include "Graphics/SlabGraphics.h"

#include "Plot2DWindow.h"

#include <ranges>

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
#include "Graphics/OpenGL/Shader.h"

namespace Slab::Graphics {

    using Log = Core::Log;

    std::map<Str, FPlot2DWindow *> FPlot2DWindow::GraphMap = {};
    CountType FPlot2DWindow::WindowCount = 0;

    using Mappy = FPlot2DWindow::ContentMap;
    bool change_z_order(Mappy& mappy, Mappy::iterator &it, int z_order){
        if(z_order == it->first) return false;

        auto artist = it->second;
        it = mappy.erase(it);
        mappy.insert(std::make_pair(z_order, artist));

        return true;
    }
    bool z_order_up  (Mappy& mappy, Mappy::iterator it) { return change_z_order(mappy, it, it->first+1); }
    bool z_order_down(Mappy& mappy, Mappy::iterator it) { return change_z_order(mappy, it, it->first-1); }

    FPlot2DWindow::FPlot2DWindow(
        DevFloat xMin,
        DevFloat xMax,
        DevFloat yMin,
        DevFloat yMax,
        Str _title,
        const Pointer<FImGuiContext>& ArgGuiContext)
    : FSlabWindow(FSlabWindowConfig(_title))
    , Id(++WindowCount)
    , Region{{xMin, xMax, yMin, yMax}}
    , Title(std::move(_title))
    , GuiContext(ArgGuiContext)
    {
        // Instantiate our dedicated Plot themes manager
        PlotThemeManager::GetInstance();

        if(this->GuiContext == nullptr)
        {
            throw Exception("Null pointer provided to GuiContext");
        }

        AxisArtist.SetLabel("Axis");
        ArtistXHair.SetLabel("X-hair");
        LabelsArtist.SetLabel("Labels");

        if (Title.empty()) Title = Str("unnamed");
        CountType n = 1;
        {
            Str UniqueTitle = Title;
            while (GraphMap.contains(UniqueTitle))
                UniqueTitle = Title + "(" + ToStr(++n) + ")";
            Title = UniqueTitle;
        }
        GraphMap[Title] = this;

        Core::LoadModule("ImGui");
        Core::LoadModule("RealTimeAnimation");

        Log::Note() << "Created PlottingWindow '" << Title << "'" << Log::Flush;

        AddArtist(Naked(AxisArtist), 5);
        AddArtist(Naked(LabelsArtist), 5);
        AddArtist(Naked(ArtistXHair), 6);
        AddArtist(Naked(bgArtist), -10);

    }

    FPlot2DWindow::FPlot2DWindow(Str title, const Pointer<FImGuiContext>& GuiContext)
    : FPlot2DWindow(-1, 1, -1, 1, std::move(title), GuiContext) {    }

    void FPlot2DWindow::AddArtist(const FArtist_ptr &pArtist, zOrder_t zOrder) {
        if (pArtist == nullptr) {
            Log::Error() << __PRETTY_FUNCTION__ << " trying to add "
                         << Log::FGBlue << "nullptr" << Log::ResetFormatting << " artist.";

            return;
        }

        Content.emplace(zOrder, pArtist);

        Log::Note()
        << "PlottingWindow '" << this->Title << "' added artist '"
        << pArtist->GetLabel() << "'." << Log::Flush;
    }

    bool FPlot2DWindow::RemoveArtist(const Graphics::FArtist_ptr &pArtist) {
        auto haveErased = false;

        for (auto item = Content.begin(); item != Content.end();) {
            if (item->second == pArtist) {
                item = Content.erase(item);
                haveErased = true;
            } else ++item;
        }

        return haveErased;
    }

    void FPlot2DWindow::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        OpenGL::CheckGLErrors(Str(__PRETTY_FUNCTION__) + "; '" + Title + "'");

        FSlabWindow::ImmediateDraw(PlatformWindow);

        if (AutoReviewGraphRanges) ReviewGraphRanges();

        ArtistsDraw();
    }

    void FPlot2DWindow::RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow)
    {
        FSlabWindow::RegisterDeferredDrawCalls(PlatformWindow);

        RegisterGUIDraws();
    }

    void FPlot2DWindow::RegisterGUIDraws() {
        auto DrawCall = [this]
        {
            auto popupName = AddUniqueIdToString(Title + Str(" window popup"));

            if (PopupOn && !POPUP_ON_MOUSE_CALL) {

                ImGui::OpenPopup(AddUniqueIdToString(popupName).c_str());
                PopupOn = false;
            }

            if (ImGui::BeginPopup(AddUniqueIdToString(popupName).c_str())) {
                if(ImGui::MenuItem("Auto adjust", nullptr, AutoReviewGraphRanges)) {
                    AutoReviewGraphRanges = !AutoReviewGraphRanges;
                }
                else if (ImGui::MenuItem(AddUniqueIdToString("Show interface").c_str(), nullptr, ShowInterface)) {
                    ShowInterface = !ShowInterface;
                } else if (ImGui::MenuItem(AddUniqueIdToString("Save graph").c_str())) {

                    auto w = Printing::getTotalHorizontalDots(.5);
                    auto h = w * .5;
                    auto fileName = Title + " " +
                                    Core::FCommandLineInterfaceManager::getInstance().renderParametersToString({"N", "L"}) +
                                    ".png";

                    OpenGL::OutputToPNG(this, fileName, w, (int) h);
                }

                ImGui::EndPopup();
            }


            bool Closable = false;

            const auto Viewport = GetViewport();
            ImGui::SetNextWindowPos(
                {
                    static_cast<float>(Viewport.xMin+200),
                    static_cast<float>(Viewport.yMin+200)
                },
                ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(
                {
                    static_cast<float>(Viewport.GetWidth()),
                    static_cast<float>(Viewport.GetHeight())
                },
                ImGuiCond_Appearing);

            ImGui::SetNextWindowBgAlpha(0.65);

            constexpr auto Flags =
                // ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                // ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoBringToFrontOnFocus;

            if (ImGui::Begin(AddUniqueIdToString("Plot Detail").c_str(), &Closable, Flags))
            {
                for (auto it = Content.begin(); it!=Content.end(); )
                {
                    IN artie = it->second;

                    bool increment_iterator = true;

                    auto label = artie->GetLabel();
                    if (ImGui::ArrowButton((label + "##up").c_str(), ImGuiDir_Up)){
                        increment_iterator = !z_order_up(Content, it);
                    }
                    ImGui::SameLine();
                    ImGui::Text("z=%i", it->first);
                    ImGui::SameLine();
                    if (ImGui::ArrowButton((label + "##down").c_str(), ImGuiDir_Down)) {
                        increment_iterator = !z_order_down(Content, it);
                    }
                    ImGui::SameLine();

                    bool visible = artie->IsVisible();
                    if (ImGui::Checkbox((AddUniqueIdToString(artie->GetLabel())+"_checkbox").c_str() , &visible)) {
                        artie->SetVisibility(visible);
                    }

                    if(increment_iterator) ++it;
                }


                auto avail_region = ImGui::GetContentRegionAvail();
                ImGui::BeginChild(AddUniqueIdToString(Title + " :)").c_str(), {avail_region.x, 0},
                                  ImGuiChildFlags_Border
                                  | ImGuiChildFlags_AutoResizeY);

                for (IN cont: Content) {
                    IN artie = cont.second;

                    if (artie->IsVisible() && artie->HasGUI()) {
                        if (ImGui::CollapsingHeader((AddUniqueIdToString(artie->GetLabel())).c_str()))
                            artie->DrawGUI();
                    }
                }

                ImGui::EndChild();
            }

            ImGui::End();
        };

        GuiContext->AddDrawCall(DrawCall);
    }

    void FPlot2DWindow::SetupOrtho() const {
        fix Viewport = GetViewport();
        fix CurrentStyle = PlotThemeManager::GetCurrent();
        CurrentStyle->LabelsWriter->Reshape(Viewport.GetWidth(), Viewport.GetHeight());
        CurrentStyle->TicksWriter->Reshape(Viewport.GetWidth(), Viewport.GetHeight());
    }

    void FPlot2DWindow::ArtistsDraw() {
        SetupOrtho();

        for (const auto& Artist : Content | std::views::values)
        {
            if (Artist->IsVisible() && !Artist->Draw(*this))
            {
                Core::Log::Error()
                << "In PlottingWindow \"" << Title
                << "\" while drawing Artist \"" << Artist->GetLabel() << "\"" << Core::Log::Flush;
            }
        }
    }

    FAxisArtist &
    FPlot2DWindow::GetAxisArtist() { return AxisArtist; }

    void
    FPlot2DWindow::SetAutoReviewGraphRanges(bool autoReview) { AutoReviewGraphRanges = autoReview; }

    void
    FPlot2DWindow::toggleShowInterface() { ShowInterface = !ShowInterface; }

    void
    FPlot2DWindow::RequireLabelOverlay(const Str& label, const Pointer<Graphics::PlotStyle>& style) const {
        const_cast<FLabelsArtist*>(&LabelsArtist)->add(label, style);
    }

    void FPlot2DWindow::TieRegion_xMaxMin(const Graphics::FPlot2DWindow &other) {
        Region.setReference_xMin(other.GetRegion().getReference_xMin());
        Region.setReference_xMax(other.GetRegion().getReference_xMax());
    }

    void FPlot2DWindow::TieRegion_yMaxMin(const Graphics::FPlot2DWindow &other) {
        Region.setReference_yMin(other.GetRegion().getReference_yMin());
        Region.setReference_yMax(other.GetRegion().getReference_yMax());
    }

    void FPlot2DWindow::RemoveArtists(const Vector<Graphics::FArtist_ptr> &artists) {
        for(const auto& artist : artists) RemoveArtist(artist);
    }

}
