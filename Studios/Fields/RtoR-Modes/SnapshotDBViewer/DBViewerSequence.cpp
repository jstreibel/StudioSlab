//
// Created by joao on 7/5/24.
//

#include "DBViewerSequence.h"

// #include <cub/detail/nvtx3.hpp>

#include "Core/Backend/BackendManager.h"

#include "KGDispersionRelation.h"

#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"

#include "Math/Function/RtoR/Model/RtoRFunctionSampler.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NativeFunction.h"

#include "Graphics/Plot2D/Plotter.h"
#include "Math/Toolset/FindPeak.h"

namespace Modes::DatabaseViewer {

    using namespace Slab;

#define z_order(z) (z)

    DBViewerSequence::DBViewerSequence(const StrVector& dbFilenames, const Str &criticalParam)
    : FWindowRow()
    , guiWindow(Graphics::FSlabWindowConfig{"GUI"})
    , mashupDisplay("All data")
    , massesGraph("masses")
    {
        for(const auto &dbFilename : dbFilenames) {
            auto parser = New<DBParser>(dbFilename, criticalParam, ".");
            dbParsers.emplace_back(parser);
        }

        this->AddWindow(Naked(guiWindow));

        {
            auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0];
            style.filled = false;
            style.setPrimitive(Slab::Graphics::PlottingSolid);
            auto funky = Math::RtoR::NativeFunction([](DevFloat x) { return x; }).Clone();
            Graphics::FPlotter::AddRtoRFunction(Naked(mashupDisplay), funky, style, "m=0", 1000, 3);
        }

        // Setup masses Re and Im pointsets.
        {
            auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[2];
            style.setPrimitive(Graphics::PlottingVerticalLinesWithCircles);
            style.filled = false;
            style.thickness = 1.5;
            Graphics::FPlotter::AddPointSet(Naked(massesGraph), Naked(massesReal_pointSet), style,
                                           "m=sqrt(ωₚₑₐₖ²-k²)", true);
            style.lineColor = style.lineColor.brg();
            Graphics::FPlotter::AddPointSet(Naked(massesGraph), Naked(massesImag_pointSet), style,
                                           "ℑ[m=√(ωₚₑₐₖ²-k²)]");
            //style.lineColor = style.lineColor.brg();
            //style.thickness = 3.0;
            //Graphics::Plotter::AddPointSet(Naked(massesGraph), Naked(underXHair), style, "under X-hair");

            massesGraph.GetAxisArtist().setVerticalAxisLabel("m");
            massesGraph.GetAxisArtist().SetHorizontalAxisLabel("k");
            massesGraph.GetAxisArtist().setHorizontalUnit(Math::Constants::π);
        }

        // Setup mashup display
        {
            mashupDisplay.GetAxisArtist().setVerticalUnit(Math::Constants::π);
            mashupDisplay.GetAxisArtist().setHorizontalUnit(Math::Constants::π);
            auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[1];
            style.thickness = 3;
            style.filled = false;
            KGRelation_artist = Graphics::FPlotter::AddPointSet(Naked(mashupDisplay), KGRelation, style,
                                                               "ω²-kₚₑₐₖ²-m²=0", false, z_order(1));

            style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0];
            style.setPrimitive(Graphics::Points);
            style.thickness = 8;
            Graphics::FPlotter::AddPointSet(Naked(mashupDisplay), Naked(maxValuesPointSet), style,
                                           "main modes", false, z_order(2));
            // allDataDisplay.setColorMap(Graphics::ColorMaps["blues"]);

            topRow.AddWindow(Naked(mashupDisplay));
        }

        // Link mashup and masses display x limits
        mashupDisplay.GetRegion().setReference_xMin(massesGraph.GetRegion().getReference_xMin());
        mashupDisplay.GetRegion().setReference_xMax(massesGraph.GetRegion().getReference_xMax());

        const TPointer<Graphics::WindowColumn> winCol(new Graphics::WindowColumn);
        winCol->addWindow(Naked(massesGraph));
        winCol->addWindow(Naked(topRow), 0.75);

        AddWindow(winCol, FWindowRow::Left, .8);

        reloadData();
    }

    void DBViewerSequence::ImmediateDraw(const Graphics::FPlatformWindow& PlatformWindow) {
        if(currentMeshupArtist == nullptr) return;

        fix ω_XHair = mashupDisplay.GetLastXHairPosition().x;
        fix dx = currentMashup->getSpace().getMetaData().geth(0);
        fix L = currentMashup->getDomain().getLx();
        fix N = currentMashup->getN();
        fix ωMin = -1.5*dx+currentMashup->getDomain().xMin;

        index_XHair =  static_cast<int>(N * (ω_XHair - ωMin) / (L + dx));

        guiWindow.AddExternalDraw([this](){
            if(ImGui::SliderInt("Current database", &current_database, 0,
                static_cast<int>(mashupArtists.size())-1)) {
                if(currentMeshupArtist != nullptr) {
                    mashupDisplay.RemoveArtist(currentMeshupArtist);
                    // mashupDisplay.removeArtist(currentMeshupArtist->getColorBarArtist());
                }

                currentMashup = allMashups[current_database];
                currentMeshupArtist = mashupArtists[current_database];
                mashupDisplay.AddArtist(currentMeshupArtist);
                // mashupDisplay.addArtist(currentMeshupArtist->getColorBarArtist());

                computeMasses();
            }

            if(ImGui::CollapsingHeader("Dominant modes")) {
                if(ImGui::SliderInt("avg range", &masses_avg_samples, 1, 12)){
                    computeMasses();
                }
                drawTable(index_XHair);

                // if(index_XHair>=0 && index_XHair<=fullFields[0]->getN()){ underXHair.addPoint({ω_XHair, }); };
            }

            if(ImGui::CollapsingHeader("Klein-Gordon dispersion relation")){
                static bool isVisible = false;

                if(ImGui::Checkbox("Visible", &isVisible))
                    updateKGDispersion(isVisible);

                auto mass = (float)KG_mass;
                if(ImGui::SliderFloat("mass##slider", &mass, 0.0, 10.0)
                   | ImGui::DragFloat("mass##drag", &mass, mass/1000.f, 0.f, 10.f)) {
                    KG_mass = mass;
                    isVisible = true;
                    updateKGDispersion(isVisible);
                }
            }
        });

        FWindowRow::ImmediateDraw(PlatformWindow);
    }

    void DBViewerSequence::updateKGDispersion(bool visible) {
        const DevFloat mass = KG_mass;

        if(!visible) return;

        auto xMax = 0.0;
        for(const auto &mashup : allMashups){
            if(const auto xMax_local = mashup->getDomain().xMax; xMax_local > xMax)
                xMax = xMax_local;
        }

        fix mainDBType = dbParsers[0]->evaluateDatabaseType();
        const auto dispersionMode = mainDBType == SpaceDFTDBType ? Slab::Math::RtoR::k_AsFunctionOf_ω
                                                           : mainDBType == TimeDFTDBType  ? Slab::Math::RtoR::ω_AsFunctionOf_k
                                                                                          : Slab::Math::RtoR::k_AsFunctionOf_ω;

        KGRelation = Math::RtoR::FunctionRenderer::ToPointSet(
                Math::RtoR::KGDispersionRelation(mass, dispersionMode),
                0.0, xMax, 10000);

        auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        style.thickness = 3;
        style.filled = false;
        KGRelation_artist->setStyle(style);
        KGRelation_artist->setPointSet(KGRelation);
        KGRelation_artist->SetLabel(Str("ω²-k²-1=0"));
    }

    void DBViewerSequence::computeMasses() {
        assert(dbParsers.size() == allMashups.size());

        maxValues.clear();
        maxValuesPointSet.Clear();
        massesImag_pointSet.Clear();
        massesReal_pointSet.Clear();

        const auto dbParser = dbParsers[current_database];
        fix db_type = dbParser->evaluateDatabaseType();

        for (auto &fieldMap = dbParser->getSnapshotMap();
             auto &entry: fieldMap) {
            IN snapshot_function1d = entry.second.snapshotData.data;
            IN data = snapshot_function1d->getSpace().getHostData();

            fix N = snapshot_function1d->N;
            fix y_min = snapshot_function1d->xMin;

            auto maxInfo = Utils::GetMax(data);

            fix dy = (snapshot_function1d->xMax - snapshot_function1d->xMin)/static_cast<DevFloat>(N - 1);

            fix idx = static_cast<int>(maxInfo.idx);
            fix order = masses_avg_samples;
            // auto y_peak = Slab::Math::nthOrderPeakPosition(data, idx, y_min, dy, order);
            // auto y_peak = Slab::Math::parabolicPeakPosition(data, idx, y_min, dy);
            // auto y_peak = Slab::Math::cubicPeakPosition(data, idx, y_min, dy);
            const auto y_peak = Slab::Math::avgPeakPosition(data, idx, y_min, dy, order);

            IN x = entry.second.getScaledCriticalParameter(); // this is the critical parameter!! The fundamental one that changes from snapshot to snapshot.

            maxValues.emplace_back(maxInfo);

            DevFloat ω;
            DevFloat k;

            maxValuesPointSet.AddPoint({x, y_peak});

            if (db_type==SpaceDFTDBType) {
                ω = x;
                k = y_peak;
            } else if (db_type==TimeDFTDBType) {
                k = x;
                ω = y_peak;
            } else NOT_IMPLEMENTED

            // massesReal_pointSet.addPoint({ω, m2});
            if (fix m2 = ω * ω - k * k; m2>=0)
                massesReal_pointSet.AddPoint({x, sqrt(m2)});
            else
                massesImag_pointSet.AddPoint({x, sqrt(-m2)});
        }
    }

    void DBViewerSequence::drawTable(int specialIndex) const {

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
        // const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        static auto outer_size_value = ImVec2(0.0f, TEXT_BASE_HEIGHT * 20);

        constexpr auto columns = 4;
        if (!ImGui::BeginTable("table1", columns, flags, outer_size_value)) return;

        const Str paramName = dbParsers[0]->getCriticalParameter();

        ImGui::TableSetupColumn((paramName + " (x axis)").c_str());
        ImGui::TableSetupColumn("Aₘₐₓ");
        ImGui::TableSetupColumn("k (y axis)");
        ImGui::TableSetupColumn("m² = ω² - k²");
        ImGui::TableSetupScrollFreeze(0, 1);

        ImGui::TableHeadersRow();

        fix unit = Math::Constants::π;
        auto &snapshotMap = dbParsers[current_database]->getSnapshotMap();
        int i=0;
        for (auto &entry : snapshotMap)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            fix criticalParameterValue = entry.second.critical_parameter_value;
            ImGui::TextUnformatted(unit(criticalParameterValue, 4).c_str());

            ImGui::TableSetColumnIndex(1);
            const auto [value, idx] = maxValues[i++];
            ImGui::TextUnformatted(ToStr(value, 5).c_str());

            ImGui::TableSetColumnIndex(2);
            IN field = *entry.second.snapshotData.data;
            fix kMax = field.xMax;
            fix kMin = field.xMin;
            fix Δk = kMax-kMin;
            fix k = Δk*static_cast<DevFloat>(idx)/static_cast<DevFloat>(field.N) - kMin;
            ImGui::TextUnformatted(unit(k, 4).c_str());

            ImGui::TableSetColumnIndex(3);
            auto type = entry.second.snapshotData.snapshotDataType;
            fix signal = type==SnapshotData::SpaceDFTSnapshot ? 1 : type==SnapshotData::TimeDFTSnapshot?-1:0;
            fix ω = entry.second.getScaledCriticalParameter();

            if(fix m2 = signal*(ω*ω - k*k); m2>=0)
                ImGui::TextUnformatted(ToStr(sqrt(m2), 4).c_str());
            else
                ImGui::TextColored({0.75f,.25f,.25f,1.f}, (ToStr(sqrt(-m2), 4)+"𝕚").c_str(), nullptr);

            if(i == specialIndex)
            {
                ImU32 line_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.9f, 0.65f));
                for(int col=0;col<columns;++col) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, line_bg_color, col);

                ImGui::SetScrollHereY(.5f);
            }

        }

        ImGui::EndTable();
    }

    auto DBViewerSequence::NotifyKeyboard(const Graphics::EKeyMap key, const Graphics::EKeyState state, const Graphics::EModKeys modKeys) -> bool {
        if( key==Graphics::Key_F5 && state==Graphics::Press ){
            reloadData();
            return true;
        }

        return FWindowRow::NotifyKeyboard(key, state, modKeys);
    }

    void DBViewerSequence::reloadData() {
        if(!allMashups.empty()) NOT_IMPLEMENTED

        for(auto &artie : mashupArtists)
            mashupDisplay.RemoveArtist(artie);
        mashupArtists.clear();

        const auto cmap = Graphics::ColorMaps["blues"]->inverse().clone();
        TPointer<Graphics::R2toRPainter> rPainter;
        for (IN dbParser: dbParsers) {
            auto mashup = dbParser->buildSnapshotMashup();
            // auto dbRootFolder = ReplaceAll(dbParser->getRootDatabaseFolder(), "./", "");
            fix dbRootFolder = dbParser->getRootDatabaseFolder();

            auto artie = New<Graphics::R2toRFunctionArtist>();
            artie->SetLabel(dbRootFolder);
            artie->setFunction(mashup);
            if(rPainter == nullptr) {
                const auto colorPainter = DynamicPointerCast<Slab::Graphics::Colormap1DPainter>(artie->getPainter("Colormap"));
                colorPainter->setColorMap(cmap);
                rPainter = colorPainter;

                auto colorBarArtist = colorPainter->getColorBarArtist();
                mashupDisplay.AddArtist(colorBarArtist, 10);
            }
            else {
                artie->setPainter(rPainter);
            }

            allMashups.emplace_back(mashup);
            mashupArtists.emplace_back(artie);
        }

        if(current_database < 0 && !mashupArtists.empty()) current_database = 0;
        if(currentMeshupArtist != nullptr)
            mashupDisplay.RemoveArtist(currentMeshupArtist);

        currentMashup = allMashups[current_database];
        currentMeshupArtist = mashupArtists[current_database];
        mashupDisplay.AddArtist(currentMeshupArtist);

        // mashupDisplay.addArtist(currentMeshupArtist->getColorBarArtist());

        if (fix db_type = dbParsers[0]->evaluateDatabaseType();
            db_type == SpaceDFTDBType) {
            mashupDisplay.GetAxisArtist().setVerticalAxisLabel("k");
            mashupDisplay.GetAxisArtist().SetHorizontalAxisLabel("ω");

            massesGraph.GetAxisArtist().SetHorizontalAxisLabel("ω");
        }
        else if (db_type == TimeDFTDBType) {
            mashupDisplay.GetAxisArtist().setVerticalAxisLabel("ω");
            mashupDisplay.GetAxisArtist().SetHorizontalAxisLabel("k");

            massesGraph.GetAxisArtist().SetHorizontalAxisLabel("k");
        }
        else throw Exception("Unknown db type");

        computeMasses();
    }

}