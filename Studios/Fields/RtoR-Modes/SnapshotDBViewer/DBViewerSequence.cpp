//
// Created by joao on 7/5/24.
//

#include "DBViewerSequence.h"

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
    : WindowRow(HasMainMenu)
    , guiWindow()
    , mashupDisplay("All data")
    , massesGraph("masses")
    {
        for(const auto &dbFilename : dbFilenames) {
            auto parser = New<Modes::DatabaseViewer::DBParser>(dbFilename, criticalParam, ".");
            dbParsers.emplace_back(parser);
        }

        this->addWindow(Naked(guiWindow));

        {
            auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0];
            style.filled = false;
            style.setPrimitive(Slab::Graphics::Solid);
            auto funky = Math::RtoR::NativeFunction([](Real x) { return x; }).Clone();
            Graphics::Plotter::AddRtoRFunction(Naked(mashupDisplay), funky, style, "m=0", 1000, 3);
        }

        // Setup masses Re and Im pointsets.
        {
            auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[2];
            style.setPrimitive(Graphics::VerticalLines);
            style.filled = false;
            style.thickness = 1.5;
            Graphics::Plotter::AddPointSet(Naked(massesGraph), Naked(massesReal_pointSet), style,
                                           "m=sqrt(ωₚₑₐₖ²-k²)", true);
            style.lineColor = style.lineColor.brg();
            Graphics::Plotter::AddPointSet(Naked(massesGraph), Naked(massesImag_pointSet), style,
                                           "ℑ[m=√(ωₚₑₐₖ²-k²)]");
            //style.lineColor = style.lineColor.brg();
            //style.thickness = 3.0;
            //Graphics::Plotter::AddPointSet(Naked(massesGraph), Naked(underXHair), style, "under X-hair");

            massesGraph.getAxisArtist().setVerticalAxisLabel("m");
            massesGraph.getAxisArtist().setHorizontalAxisLabel("k");
            massesGraph.getAxisArtist().setHorizontalUnit(Math::Constants::π);
        }

        // Setup mashup display
        {
            mashupDisplay.getAxisArtist().setVerticalUnit(Math::Constants::π);
            mashupDisplay.getAxisArtist().setHorizontalUnit(Math::Constants::π);
            auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
            style.thickness = 3;
            style.filled = false;
            KGRelation_artist = Graphics::Plotter::AddPointSet(Naked(mashupDisplay), KGRelation, style,
                                                               "ω²-kₚₑₐₖ²-m²=0", false, z_order(1));

            style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0];
            style.setPrimitive(Graphics::Point);
            style.thickness = 8;
            Graphics::Plotter::AddPointSet(Naked(mashupDisplay), Naked(maxValuesPointSet), style,
                                           "main modes", false, z_order(2));
            // allDataDisplay.setColorMap(Graphics::ColorMaps["blues"]);

            mashupDisplay.getAxisArtist().setVerticalAxisLabel("ω");
            mashupDisplay.getAxisArtist().setHorizontalAxisLabel("k");

            topRow.addWindow(Naked(mashupDisplay));
        }

        // Link mashup and masses display x limits
        mashupDisplay.getRegion().setReference_xMin(massesGraph.getRegion().getReference_xMin());
        mashupDisplay.getRegion().setReference_xMax(massesGraph.getRegion().getReference_xMax());

        Pointer<Graphics::WindowColumn> winCol(new Graphics::WindowColumn);
        winCol->addWindow(Naked(massesGraph));
        winCol->addWindow(Naked(topRow), 0.75);

        addWindow(winCol, WindowRow::Left, .8);

        reloadData();
    }

    void DBViewerSequence::draw() {
        if(currentMeshupArtist == nullptr) return;

        fix ω_XHair = mashupDisplay.getLastXHairPosition().x;
        fix dx = currentMashup->getSpace().getMetaData().geth(0);
        fix L = currentMashup->getDomain().getLx();
        fix N = currentMashup->getN();
        fix ωMin = -1.5*dx+currentMashup->getDomain().xMin;

        index_XHair =  (int)(N*(ω_XHair-ωMin)/(L+dx));

        guiWindow.begin();

        if(ImGui::SliderInt("Current database", &current_database, 0, mashupArtists.size()-1)) {
            if(currentMeshupArtist != nullptr) {
                mashupDisplay.removeArtist(currentMeshupArtist);
                // mashupDisplay.removeArtist(currentMeshupArtist->getColorBarArtist());
            }

            currentMashup = allMashups[current_database];
            currentMeshupArtist = mashupArtists[current_database];
            mashupDisplay.addArtist(currentMeshupArtist);
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
        guiWindow.end();

        WindowRow::draw();
    }

    void DBViewerSequence::updateKGDispersion(bool visible) {
        Real mass = KG_mass;

        if(!visible) return;

        auto xMax = 0.0;
        for(auto &mashup : allMashups){
            auto xMax_local = mashup->getDomain().xMax;
            if(xMax_local > xMax) xMax = xMax_local;
        }

        fix mainDBType = dbParsers[0]->evaluateDatabaseType();
        auto dispersionMode = mainDBType == SpaceDFTDBType ? Slab::Math::RtoR::k_AsFunctionOf_ω
                                                           : mainDBType == TimeDFTDBType  ? Slab::Math::RtoR::ω_AsFunctionOf_k
                                                                                          : Slab::Math::RtoR::k_AsFunctionOf_ω;

        KGRelation = Math::RtoR::FunctionRenderer::ToPointSet(
                Math::RtoR::KGDispersionRelation(mass, dispersionMode),
                0.0, xMax, 10000);

        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
        style.thickness = 3;
        style.filled = false;
        KGRelation_artist->setStyle(style);
        KGRelation_artist->setPointSet(KGRelation);
        KGRelation_artist->setLabel(Str("ω²-k²-1=0"));
    }

    void DBViewerSequence::computeMasses() {
        assert(dbParsers.size() == allMashups.size());

        maxValues.clear();
        maxValuesPointSet.clear();
        massesImag_pointSet.clear();
        massesReal_pointSet.clear();

        auto dbParser = dbParsers[current_database];

        auto &fieldMap = dbParser->getSnapshotMap();

        for (auto &entry: fieldMap) {
            IN snapshot_function1d = entry.second.snapshotData.data;
            IN data = snapshot_function1d->getSpace().getHostData();

            fix N = snapshot_function1d->N;
            fix y_min = snapshot_function1d->xMin;

            auto maxInfo = Utils::GetMax(data);

            fix dy = (snapshot_function1d->xMax - snapshot_function1d->xMin)/(Real)(N-1);

            fix idx = (int)maxInfo.idx;
            fix order = masses_avg_samples;
            // auto y_peak = Slab::Math::nthOrderPeakPosition(data, idx, y_min, dy, order);
            // auto y_peak = Slab::Math::parabolicPeakPosition(data, idx, y_min, dy);
            // auto y_peak = Slab::Math::cubicPeakPosition(data, idx, y_min, dy);
            auto y_peak = Slab::Math::avgPeakPosition(data, idx, y_min, dy, order);

            IN x = entry.second.getScaledCriticalParameter(); // this is the critical parameter!! The fundamental one that changes from snapshot to snapshot.

            maxValues.emplace_back(maxInfo);
            fix k = x;
            fix ω = y_peak;
            maxValuesPointSet.addPoint({k, ω});


            fix m2 = ω * ω - k * k;

            // massesReal_pointSet.addPoint({ω, m2});
            if (m2>=0)
                massesReal_pointSet.addPoint({k, sqrt(m2)});
            else
                massesImag_pointSet.addPoint({k, sqrt(-m2)});
        }
    }

    void DBViewerSequence::drawTable(int specialIndex) {

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
        // const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        static ImVec2 outer_size_value = ImVec2(0.0f, TEXT_BASE_HEIGHT * 20);

        fix columns = 4;
        if (!ImGui::BeginTable("table1", columns, flags, outer_size_value)) return;

        Str paramName = dbParsers[0]->getCriticalParameter();

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
            fix maxData = maxValues[i++];
            ImGui::TextUnformatted(ToStr(maxData.value, 5).c_str());

            ImGui::TableSetColumnIndex(2);
            fix idx = maxData.idx;
            IN field = *entry.second.snapshotData.data;
            fix kMax = field.xMax;
            fix kMin = field.xMin;
            fix Δk = kMax-kMin;
            fix k = Δk*(Real)idx/(Real)field.N - kMin;
            ImGui::TextUnformatted(unit(k, 4).c_str());

            ImGui::TableSetColumnIndex(3);
            auto type = entry.second.snapshotData.snapshotDataType;
            fix signal = type==SnapshotData::SpaceDFTSnapshot ? 1 : type==SnapshotData::TimeDFTSnapshot?-1:0;
            fix ω = entry.second.getScaledCriticalParameter();

            fix m2 = signal*(ω*ω - k*k);
            if(m2>=0)
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

    auto DBViewerSequence::notifyKeyboard(Graphics::KeyMap key, Graphics::KeyState state, Graphics::ModKeys modKeys) -> bool {
        if( key==Graphics::Key_F5 && state==Graphics::Press ){
            reloadData();
            return true;
        }

        return WindowRow::notifyKeyboard(key, state, modKeys);
    }

    void DBViewerSequence::reloadData() {
        if(!allMashups.empty()) NOT_IMPLEMENTED

        for(auto &artie : mashupArtists)
            mashupDisplay.removeArtist(artie);
        mashupArtists.clear();

        auto cmap = Graphics::ColorMaps["blues"]->inverse().clone();
        Pointer<Graphics::R2toRPainter> rPainter;
        Pointer<Graphics::OpenGL::ColorBarArtist> colorBarArtist = nullptr;
        for (auto &dbParser: dbParsers) {
            auto mashup = dbParser->buildSnapshotMashup();
            // auto dbRootFolder = ReplaceAll(dbParser->getRootDatabaseFolder(), "./", "");
            auto dbRootFolder = dbParser->getRootDatabaseFolder();

            auto artie = New<Graphics::R2toRFunctionArtist>();
            artie->setLabel(dbRootFolder);
            artie->setFunction(mashup);
            // artie->setColorMap(cmap);
            if(rPainter == nullptr) {
                rPainter = artie->getPainter();
            }
            else {
                artie->setPainter(rPainter);
            }

            allMashups.emplace_back(mashup);
            mashupArtists.emplace_back(artie);
        }

        if(current_database < 0 && !mashupArtists.empty()) current_database = 0;
        if(currentMeshupArtist != nullptr)
            mashupDisplay.removeArtist(currentMeshupArtist);

        currentMashup = allMashups[current_database];
        currentMeshupArtist = mashupArtists[current_database];
        mashupDisplay.addArtist(currentMeshupArtist);

        // mashupDisplay.addArtist(currentMeshupArtist->getColorBarArtist());

        computeMasses();
    }

}