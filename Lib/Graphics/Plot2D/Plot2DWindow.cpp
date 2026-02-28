//
// Created by joao on 29/05/23.
//

#include "Core/Tools/Log.h"

#include "Graphics/SlabGraphics.h"

#include "Plot2DWindow.h"

#include <ranges>

#include "PlotStyle.h"

#include <utility>

#include "3rdParty/ImGui.h"

#include "Utils/Printing.h"

#include "../../Core/Controller/InterfaceManager.h"

#include "PlotThemeManager.h"
#include "Artists/AxisArtist.h"

#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Core/SlabCore.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "StudioSlab.h"
#include "Graphics/OpenGL/Shader.h"

namespace Slab::Graphics {

    using Core::FLog;

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
        Str _title)
    : FSlabWindow(FSlabWindowConfig(_title))
    , Id(++WindowCount)
    , Region{{xMin, xMax, yMin, yMax}}
    , Title(std::move(_title))
    {
        // Instantiate our dedicated Plot themes manager
        FPlotThemeManager::GetInstance();

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

        FLog::Note() << "Created PlottingWindow '" << Title << "'" << FLog::Flush;

        AddArtist(Naked(AxisArtist), 5);
        AddArtist(Naked(LabelsArtist), 5);
        AddArtist(Naked(ArtistXHair), 6);
        AddArtist(Naked(bgArtist), -10);

    }

    FPlot2DWindow::FPlot2DWindow(Str Title)
    : FPlot2DWindow(-1, 1, -1, 1, std::move(Title)) {    }

    void FPlot2DWindow::AddArtist(const FArtist_ptr &pArtist, zOrder_t zOrder) {
        if (pArtist == nullptr) {
            FLog::Error() << __PRETTY_FUNCTION__ << " trying to add "
                         << FLog::FGBlue << "nullptr" << FLog::ResetFormatting << " artist.";

            return;
        }

        Content.emplace(zOrder, pArtist);

        FLog::Note()
        << "PlottingWindow '" << this->Title << "' added artist '"
        << pArtist->GetLabel() << "'." << FLog::Flush;
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

    void FPlot2DWindow::SetNoGUI() {
        b_DrawGUI = false;
    }

    void FPlot2DWindow::RegisterGUIDraws() {
        auto DrawCall = [this]
        {
            const auto Viewport = GetViewport();
            if (Viewport.GetWidth() <= 0 || Viewport.GetHeight() <= 0) return;

            auto SaveGraphToDisk = [this]() {
                auto w = Printing::GetTotalHorizontalDots(.5);
                auto h = w * .5;
                auto fileName = Title + " " +
                                Core::FInterfaceManager::GetInstance().RenderParametersToString({"N", "L"}) +
                                ".png";

                OpenGL::OutputToPNG(this, fileName, w, (int) h);
            };

            const auto popupName = AddUniqueIdToString(Title + Str(" window popup"));
            const bool bOpenPopupFromMouseCall = PopupOn && !POPUP_ON_MOUSE_CALL;
            PopupOn = false;

            constexpr float Margin = 8.0f;
            constexpr float ToolbarMinHeight = 28.0f;

            float toolbarHeight = ToolbarMinHeight;

            ImGui::SetNextWindowPos(
                ImVec2{
                    static_cast<float>(Viewport.xMin) + Margin,
                    static_cast<float>(Viewport.yMin) + Margin
                },
                ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.82f);

            constexpr auto ToolbarFlags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_AlwaysAutoResize;

            if (ImGui::Begin(AddUniqueIdToString("Plot Toolbar").c_str(), nullptr, ToolbarFlags)) {
                if (ImGui::SmallButton("Fit")) {
                    ReviewGraphRanges();
                }
                ImGui::SameLine();
                ImGui::Checkbox("Auto", &AutoReviewGraphRanges);
                ImGui::SameLine();
                if (ImGui::SmallButton(ShowInterface ? "Hide UI" : "Show UI")) {
                    ShowInterface = !ShowInterface;
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Save")) {
                    SaveGraphToDisk();
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("...")) {
                    ImGui::OpenPopup(popupName.c_str());
                }
                if (bOpenPopupFromMouseCall) {
                    ImGui::OpenPopup(popupName.c_str());
                }

                if (ImGui::BeginPopup(popupName.c_str())) {
                    if (ImGui::MenuItem("Auto adjust", nullptr, AutoReviewGraphRanges)) {
                        AutoReviewGraphRanges = !AutoReviewGraphRanges;
                    }
                    if (ImGui::MenuItem("Show interface", nullptr, ShowInterface)) {
                        ShowInterface = !ShowInterface;
                    }
                    if (ImGui::MenuItem("Fit now")) {
                        ReviewGraphRanges();
                    }
                    if (ImGui::MenuItem("Save graph")) {
                        SaveGraphToDisk();
                    }
                    ImGui::EndPopup();
                }

                toolbarHeight = std::max(toolbarHeight, ImGui::GetWindowHeight());
            }
            ImGui::End();

            if (!ShowInterface) return;

            const float panelWidth = std::clamp(
                static_cast<float>(Viewport.GetWidth()) * 0.30f,
                300.0f,
                460.0f);
            const float panelHeight = std::max(
                200.0f,
                static_cast<float>(Viewport.GetHeight()) - toolbarHeight - (3.0f * Margin));
            const float panelX = static_cast<float>(Viewport.xMax) - panelWidth - Margin;
            const float panelY = static_cast<float>(Viewport.yMin) + toolbarHeight + (2.0f * Margin);

            ImGui::SetNextWindowPos(ImVec2{panelX, panelY}, ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2{panelWidth, panelHeight}, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.88f);

            constexpr auto DetailFlags =
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings;

            if (ImGui::Begin(AddUniqueIdToString("Plot Detail").c_str(), nullptr, DetailFlags))
            {
                if (ImGui::CollapsingHeader("View", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Checkbox("Auto adjust", &AutoReviewGraphRanges);
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Fit now")) {
                        ReviewGraphRanges();
                    }
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Save graph")) {
                        SaveGraphToDisk();
                    }

                    const auto regionRect = Region.getRect();
                    ImGui::Text("x:[%.4g, %.4g]  y:[%.4g, %.4g]",
                        regionRect.xMin,
                        regionRect.xMax,
                        regionRect.yMin,
                        regionRect.yMax);
                    ImGui::Text("w=%.4g  h=%.4g", regionRect.GetWidth(), regionRect.GetHeight());
                }

                if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto it = Content.begin(); it!=Content.end(); )
                    {
                        IN artie = it->second;
                        bool incrementIterator = true;

                        ImGui::PushID(artie.get());
                        if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
                            incrementIterator = !z_order_up(Content, it);
                        }
                        ImGui::SameLine();
                        if (ImGui::ArrowButton("##down", ImGuiDir_Down)) {
                            incrementIterator = !z_order_down(Content, it);
                        }
                        ImGui::SameLine();
                        bool visible = artie->IsVisible();
                        if (ImGui::Checkbox("##visible", &visible)) {
                            artie->SetVisibility(visible);
                        }
                        ImGui::SameLine();
                        ImGui::TextUnformatted(artie->GetLabel().c_str());
                        ImGui::SameLine();
                        ImGui::TextDisabled("(z=%i)", it->first);
                        ImGui::PopID();

                        if (incrementIterator) ++it;
                    }
                }

                if (ImGui::CollapsingHeader("Artist Controls", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    const auto availRegion = ImGui::GetContentRegionAvail();
                    ImGui::BeginChild(
                        AddUniqueIdToString(Title + " details").c_str(),
                        ImVec2{availRegion.x, 0},
                        ImGuiChildFlags_Border);

                    for (IN cont: Content) {
                        IN artie = cont.second;

                        if (artie->IsVisible() && artie->HasGUI()) {
                            if (ImGui::CollapsingHeader(AddUniqueIdToString(artie->GetLabel()).c_str())) {
                                artie->DrawGUI();
                            }
                        }
                    }

                    ImGui::EndChild();
                }
            }

            ImGui::End();
        };

        if (b_DrawGUI) DrawCall();
    }

    void FPlot2DWindow::SetupOrtho() const {
        fix Viewport = GetViewport();
        fix CurrentStyle = FPlotThemeManager::GetCurrent();
        CurrentStyle->LabelsWriter->Reshape(Viewport.GetWidth(), Viewport.GetHeight());
        CurrentStyle->TicksWriter->Reshape(Viewport.GetWidth(), Viewport.GetHeight());
    }

    void FPlot2DWindow::ArtistsDraw() {
        SetupOrtho();

        for (const auto& Artist : Content | std::views::values)
        {
            if (Artist->IsVisible() && !Artist->Draw(*this))
            {
                Core::FLog::Error()
                << "In PlottingWindow \"" << Title
                << "\" while drawing Artist \"" << Artist->GetLabel() << "\"" << Core::FLog::Flush;
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
    FPlot2DWindow::RequireLabelOverlay(const Str& label, const TPointer<Graphics::PlotStyle>& style) const {
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
