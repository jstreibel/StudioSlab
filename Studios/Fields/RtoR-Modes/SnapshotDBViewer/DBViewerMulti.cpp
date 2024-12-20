//
// Created by joao on 21/09/23.
//

#include "DBViewerMulti.h"

#include "3rdParty/ImGui.h"

#include "Core/Backend/BackendManager.h"

#include "KGDispersionRelation.h"
#include "../../Viewers/HistoryFileLoader.h"

#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"
#include "Math/Function/RtoR/Model/RtoRFunctionSampler.h"

#include "Graphics/Plot2D/Plotter.h"

namespace Modes::DatabaseViewer {

    using namespace Slab;

    #define z_order(z) (z)

    DBViewerMulti::DBViewerMulti(const StrVector& dbFilenames, const Str &criticalParam)
    : WindowRow()
    , guiWindow()
    , allDataDisplay("All data")
    , fullParticularHistoryDisplay("Particular data")
    , massesGraph("masses")
    {
        for(const auto &dbFilename : dbFilenames){
            auto parser = New<Modes::DatabaseViewer::DBParser>(dbFilename, criticalParam);
            dbParsers.emplace_back(parser);
        }

        this->addWindow(Naked(guiWindow));

        Pointer<Graphics::WindowColumn> winCol(new Graphics::WindowColumn);

        {
            auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[2];
            style.setPrimitive(Graphics::VerticalLines);
            style.filled = false;
            style.thickness = 1.5;
            Graphics::Plotter::AddPointSet(Naked(massesGraph), Naked(massesReal_pointSet), style,
                                           "ℜ[m=√(ω²-kₚₑₐₖ²)]", true);
            style.lineColor = style.lineColor.brg();
            Graphics::Plotter::AddPointSet(Naked(massesGraph), Naked(massesImag_pointSet), style,
                                           "ℑ[m=√(ω²-kₚₑₐₖ²)]");
            //style.lineColor = style.lineColor.brg();
            //style.thickness = 3.0;
            //Graphics::Plotter::AddPointSet(Naked(massesGraph), Naked(underXHair), style, "under X-hair");

            massesGraph.getAxisArtist().setHorizontalUnit(Math::Constants::π);
        }

        {
            allDataDisplay.getAxisArtist().setVerticalUnit(Math::Constants::π);
            allDataDisplay.getAxisArtist().setHorizontalUnit(Math::Constants::π);
            auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
            style.thickness = 3;
            style.filled = false;
            KGRelation_artist = Graphics::Plotter::AddPointSet(Naked(allDataDisplay), KGRelation, style,
                                                               "ω²-kₚₑₐₖ²-m²=0", false, z_order(1));
            style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
            style.thickness = 3;
            style.filled = false;
            KGRelation_high_k_artist = Graphics::Plotter::AddPointSet(Naked(allDataDisplay), KGRelation_high_k,
                                                                      style, "k=ω-½m²/ω+...", false, z_order(2));

            style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0];
            style.setPrimitive(Graphics::Point);
            style.thickness = 8;
            Graphics::Plotter::AddPointSet(Naked(allDataDisplay), Naked(maxValuesPointSet), style,
                                           "main modes", false, z_order(2));
            // allDataDisplay.setColorMap(Graphics::ColorMaps["blues"]);

            allDataDisplay.getAxisArtist().setVerticalAxisLabel("ω");
            allDataDisplay.getAxisArtist().setHorizontalAxisLabel("k");

            topRow.addWindow(Naked(allDataDisplay));
        }

        {
            currentFullParticularHistoryArtist = Graphics::Plotter::AddR2toRFunction(
                    Naked(fullParticularHistoryDisplay), nullptr, "");
            // currentFullParticularHistoryArtist->setColorMap(Graphics::ColorMaps["BrBG"]->inverse().clone());
            // topRow.addWindow(DummyPtr(fullParticularHistoryDisplay));
        }

        allDataDisplay.getRegion().setReference_xMin(massesGraph.getRegion().getReference_xMin());
        allDataDisplay.getRegion().setReference_xMax(massesGraph.getRegion().getReference_xMax());

        winCol->addWindow(Naked(massesGraph));
        winCol->addWindow(Naked(topRow), 0.75);

        addWindow(winCol, WindowRow::Left, .8);

        reloadData();
    }

    void DBViewerMulti::draw() {

        fix ω_XHair = allDataDisplay.getLastXHairPosition().x;
        fix dx = fullFields[0]->getSpace().getMetaData().geth(0);
        fix L = fullFields[0]->getDomain().getLx();
        fix N = fullFields[0]->getN();
        fix ωMin = -1.5*dx+fullFields[0]->getDomain().xMin;

        index_XHair =  (int)(N*(ω_XHair-ωMin)/(L+dx));

        guiWindow.AddExternalDraw([this](){
            if(ImGui::CollapsingHeader("Dominant modes")) {
                if(ImGui::SliderInt("avg range", &masses_avg_samples, 1, 100)){
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

        WindowRow::draw();
    }

    void DBViewerMulti::updateKGDispersion(bool visible) {
        Real mass = KG_mass;

        if(!visible) return;

        auto xMax = 0.0;
        for(auto &fullField : fullFields){
            auto xMax_local = fullField->getDomain().xMax;
            if(xMax_local > xMax) xMax = xMax_local;
        }

        fix mainDBType = dbParsers[0]->evaluateDatabaseType();
        auto dispersionMode = mainDBType == SpaceDFTDBType ? Slab::Math::RtoR::k_AsFunctionOf_ω
                            : mainDBType == TimeDFTDBType  ? Slab::Math::RtoR::ω_AsFunctionOf_k
                            : Slab::Math::RtoR::k_AsFunctionOf_ω;

        KGRelation = Math::RtoR::FunctionRenderer::ToPointSet(
                Math::RtoR::KGDispersionRelation(mass, dispersionMode),
                0.0, xMax, 10000);
        KGRelation_high_k = Math::RtoR::FunctionRenderer::ToPointSet(
                Math::RtoR::KGDispersionRelation_high_k(mass, dispersionMode),
                0.0, xMax, 10000);

        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
        style.thickness = 3;
        style.filled = false;
        KGRelation_artist->setStyle(style);
        KGRelation_artist->setPointSet(KGRelation);
        KGRelation_artist->setLabel(Str("ω²-kₚₑₐₖ²-m²=0   (Klein-Gordon with m=") + ToStr(mass) + ")");

        style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[2];
        style.thickness = 3;
        style.filled = false;
        KGRelation_high_k_artist->setStyle(style);
        KGRelation_high_k_artist->setPointSet(KGRelation_high_k);
        KGRelation_high_k_artist->setLabel(Str("k=ω-½m²/ω+...   (Klein-Gordon high-k approx with m=") + ToStr(mass) + ")");
    }

    void DBViewerMulti::computeMasses() {
        maxValues.clear();
        maxValuesPointSet.clear();
        massesImag_pointSet.clear();
        massesReal_pointSet.clear();

        for(auto &dbParser : dbParsers) {

            auto &fieldMap = dbParser->getSnapshotMap();

            for (auto &entry: fieldMap) {
                IN field = entry.second.snapshotData.data;
                IN data = field->getSpace().getHostData();

                fix N = field->N;
                fix kMin = field->xMin;

                auto maxInfo = Utils::GetMax(data);

                IN ω = entry.second.getScaledCriticalParameter(); // this is the critical parameter!! The fundamental one that changes from snapshot to snapshot.
                fix idx = (int)maxInfo.idx;
                fix Δk = field->xMax - field->xMin;

                auto k_avg = 0.0;
                auto norm = 0.0;
                fix half_samples = masses_avg_samples/2;
                fix leftover_samples = masses_avg_samples%2;
                int i = -half_samples;
                for (; i < half_samples+leftover_samples; ++i) {
                    int curr_idx = idx + i;
                    if (curr_idx < 0 || curr_idx >= N) continue;

                    auto weight = data[curr_idx];
                    auto k = Δk * (Real) (curr_idx) / (Real) N - kMin;

                    k_avg += k * weight;
                    norm += weight;
                }

                auto k = k_avg / norm;

                maxValues.emplace_back(maxInfo);
                maxValuesPointSet.addPoint({ω, k});

                fix m² = ω * ω - k * k;

                if (m²>=0)
                massesReal_pointSet.addPoint({ω, sqrt(m²)});
                else
                massesImag_pointSet.addPoint({ω, sqrt(-m²)});
            }
        }
    }

    void DBViewerMulti::drawTable(int specialIndex) {

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
        auto &fieldMap = dbParsers[0]->getSnapshotMap();
        int i=0;
        for (auto &entry : fieldMap)
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

            fix m² = signal*(ω*ω - k*k);
            if(m²>=0)
                ImGui::TextUnformatted(ToStr(sqrt(m²), 4).c_str());
            else
                ImGui::TextColored({0.75f,.25f,.25f,1.f}, (ToStr(sqrt(-m²), 4)+"𝕚").c_str(), nullptr);

            if(i == specialIndex)
            {
                ImU32 line_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.9f, 0.65f));
                for(int col=0;col<columns;++col) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, line_bg_color, col);

                ImGui::SetScrollHereY(.5f);
            }

        }

        ImGui::EndTable();
    }

    auto DBViewerMulti::notifyKeyboard(Graphics::KeyMap key, Graphics::KeyState state, Graphics::ModKeys modKeys) -> bool {
        if( key==Graphics::Key_LEFT_SHIFT  ) shiftKey = state;

        if( key==Graphics::Key_F5 && state==Graphics::Press ){
            reloadData();
            return true;        }
        else if( key==Graphics::Key_ESCAPE && state==Graphics::Press ){
            topRow.removeWindow(Naked(fullParticularHistoryDisplay));
            return true;
        }

        return WindowRow::notifyKeyboard(key, state, modKeys);
    }

    auto DBViewerMulti::notifyMouseButton(Graphics::MouseButton button, Graphics::KeyState state, Graphics::ModKeys keys) -> bool {
        static Timer timer;
        auto elTime = timer.getElTime_msec();
        if(button==Graphics::MouseButton_LEFT){
            if(state==Graphics::Press) timer.reset();
            else if(state==Graphics::Release && elTime < 200 && keys.Mod_Ctrl) {
                loadDataUnderMouse();
                return true;
            }
        }

        return WindowRow::notifyMouseButton(button, state, keys);
    }

    void DBViewerMulti::reloadData() {
        if(!fullFields.empty()) NOT_IMPLEMENTED_CLASS_METHOD;

        for(auto &artie : fullFieldsArtist)
            allDataDisplay.removeArtist(artie);
        fullFieldsArtist.clear();

        for (auto &dbParser: dbParsers) {
            auto fullField = dbParser->buildSnapshotMashup();
            // auto dbRootFolder = ReplaceAll(dbParser->getRootDatabaseFolder(), "./", "");
            auto dbRootFolder = dbParser->getRootDatabaseFolder();

            auto &cmap = Graphics::ColorMaps["blues"];
            auto artie = Graphics::Plotter::AddR2toRFunction(Naked(allDataDisplay), fullField, dbRootFolder);
            // artie->setColorMap(cmap->clone());

            fullFields.emplace_back(fullField);
            fullFieldsArtist.emplace_back(artie);
        }

        computeMasses();
    }

    void DBViewerMulti::loadDataUnderMouse() {
        auto &fSet = dbParsers[0]->getFileSet();

        auto index = index_XHair-1;
        if(index<0 || index>=fSet.size()) return;

        Vector<Pair<Real, Str>> files(fSet.begin(), fSet.end());
        auto filename = files[index].second;

        ReplaceLastOccurrence(filename, ".dft.simsnap", ".oscb");
        ReplaceLastOccurrence(filename, ".time", "");
        ReplaceLastOccurrence(filename, "snapshots/", "./");

        using Log = Core::Log;

        Log::Info() << "Looking for history..." << Log::Flush;
        auto fieldHistory = fullHistoriesMap[filename];

        if(fieldHistory == nullptr) {
            Log::Info() << "Not found. Loading history..." << Log::Flush;
            fieldHistory = HistoryFileLoader::Load(filename);
            fullHistoriesMap[filename] = fieldHistory;
        }
        Log::Info() << "Set function" << Log::Flush;
        ReplaceAll(filename, ".oscb", "");
        auto omegaStr = Split(filename, " ").back();
        omegaStr = Split(omegaStr, "=")[1];

        currentFullParticularHistoryArtist->setFunction(fieldHistory);
        currentFullParticularHistoryArtist->setLabel(Str("ω=") + omegaStr);

        Log::Info() << "Add window" << Log::Flush;
        topRow.addWindow(Naked(fullParticularHistoryDisplay));

        Log::Info() << "Done\n" << Log::Flush;
    }

    auto DBViewerMulti::notifyMouseMotion(int x, int y, int dx, int dy) -> bool {
        if( shiftKey == Graphics::Press ){
            loadDataUnderMouse();
            return true;
        };

        return WindowRow::notifyMouseMotion(x, y, dx, dy);
    }

}