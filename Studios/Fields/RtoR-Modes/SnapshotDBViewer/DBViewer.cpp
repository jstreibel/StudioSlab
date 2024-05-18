//
// Created by joao on 21/09/23.
//

#include "DBViewer.h"

#include "3rdParty/imgui/imgui.h"

#include "Core/Backend/BackendManager.h"

#include "KGDispersionRelation.h"
#include "HistoryFileLoader.h"

#include "Graphics/Graph/StylesManager.h"
#include "Graphics/OpenGL/Artists/ColorBarArtist.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"
#include "Math/Function/RtoR/Model/RtoRFunctionRenderer.h"


#include <utility>

namespace Modes::DatabaseViewer {

    DBViewer::DBViewer(StrVector dbFilenames, const Str &criticalParam)
    : WindowRow(HasMainMenu)
    , guiWindow()
    , allDataDisplay("All data")
    , fullParticularHistoryDisplay("Particular data")
    {
        for(const auto &dbFilename : dbFilenames){
            auto parser = std::make_shared<Modes::DatabaseViewer::DBParser>(dbFilename, criticalParam);
            dbParsers.emplace_back(parser);
        }


        this->addWindow(DummyPtr(guiWindow));

        std::shared_ptr<Graphics::WindowColumn> winCol(new Graphics::WindowColumn);


        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[2];
        style.primitive = Styles::VerticalLines;
        style.filled = false;
        style.thickness = 1.5;
        massesGraph.addPointSet(DummyPtr(massesReal_pointSet), style, "ℜ[m=√(ω²-kₚₑₐₖ²)]", true);
        style.lineColor = style.lineColor.permute(true);
        massesGraph.addPointSet(DummyPtr(massesImag_pointSet), style, "ℑ[m=√(ω²-kₚₑₐₖ²)]");
        style.lineColor = style.lineColor.permute();
        style.thickness = 3.0;
        massesGraph.addPointSet(DummyPtr(underXHair), style);
        massesGraph.getAxisArtist().setHorizontalUnit(Constants::π);

        allDataDisplay.getAxisArtist().setVerticalUnit(Constants::π);
        allDataDisplay.getAxisArtist().setHorizontalUnit(Constants::π);
        allDataDisplay.setColorMap(Styles::ColorMaps["blues"]);

        topRow.addWindow(DummyPtr(allDataDisplay));
        fullParticularHistoryDisplay.setColorMap(Styles::ColorMaps["BrBG"].inverse());
        // topRow.addWindow(DummyPtr(fullParticularHistoryDisplay));

        winCol->addWindow(DummyPtr(massesGraph));
        winCol->addWindow(DummyPtr(topRow), 0.75);

        addWindow(winCol, WindowRow::Left, .8);

        reloadData();
    }

    void DBViewer::draw() {

        fix ω_XHair = allDataDisplay.getLastXHairPosition().x;
        fix dx = fullFields[0]->getSpace().getMetaData().geth(0);
        fix L = fullFields[0]->getDomain().getLx();
        fix N = fullFields[0]->getN();
        fix ωMin = -1.5*dx+fullFields[0]->getDomain().xMin;

        index_XHair =  (int)(N*(ω_XHair-ωMin)/(L+dx));

        guiWindow.begin();

        if(ImGui::CollapsingHeader("Dominant modes")) {
            static bool isVisible = false;

            if(ImGui::Checkbox("Visible##dominant_modes", &isVisible)){
                if(isVisible) drawDominantModes();
                else allDataDisplay.removePointSet(DummyPtr(maxValuesPointSet));
            }

            drawTable(index_XHair);

            // if(index_XHair>=0 && index_XHair<=fullFields[0]->getN()){ underXHair.addPoint({ω_XHair, }); };
        }

        if(ImGui::CollapsingHeader("Klein-Gordon dispersion relation")){
            static bool isVisible = false;

            if(ImGui::Checkbox("Visible", &isVisible))
                updateKGDispersion(isVisible);

            auto mass = (float)KG_mass;
            if(ImGui::SliderFloat("mass", &mass, 0.0, 10.0)) {
                KG_mass = mass;
                isVisible = true;
                updateKGDispersion(isVisible);
            }

        }
        guiWindow.end();

        if(ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("System")){
                if(ImGui::MenuItem("Leave")) Core::BackendManager::GetBackend().finish();
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        WindowRow::draw();
    }

    void DBViewer::updateKGDispersion(bool visible) {
        Real mass = KG_mass;

        static Spaces::PointSet::Ptr KGRelation;
        static Spaces::PointSet::Ptr KGRelation_high_k;
        allDataDisplay.removePointSet(KGRelation);
        allDataDisplay.removePointSet(KGRelation_high_k);

        if(!visible) return;

        auto xMax = 0.0;
        for(auto &fullField : fullFields){
            auto xMax_local = fullField->getDomain().xMax;
            if(xMax_local > xMax) xMax = xMax_local;
        }
        KGRelation = RtoR::FunctionRenderer::ToPointSet(
                RtoR::KGDispersionRelation(mass, RtoR::KGDispersionRelation::k_AsFunctionOf_ω),
                0.0, xMax, 10000);
        KGRelation_high_k = RtoR::FunctionRenderer::ToPointSet(
                RtoR::KGDispersionRelation_high_k(mass, RtoR::KGDispersionRelation_high_k::k_AsFunctionOf_ω),
                0.0, xMax, 10000);

        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[1];
        style.thickness = 3;
        style.filled = false;
        allDataDisplay.addPointSet(KGRelation, style, Str("ω²-kₚₑₐₖ²-m²=0   (Klein-Gordon with m=") + ToStr(mass) + ")");

        style = Math::StylesManager::GetCurrent()->funcPlotStyles[2];
        style.thickness = 3;
        style.filled = false;
        allDataDisplay.addPointSet(KGRelation_high_k, style, Str("k=ω-½m²/ω+...   (Klein-Gordon high-k approx with m=") + ToStr(mass) + ")");
    }

    void DBViewer::drawDominantModes() {
        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[0];
        style.primitive = Styles::Point;
        style.thickness = 8;
        allDataDisplay.removePointSet("main modes");
        allDataDisplay.addPointSet(DummyPtr(maxValuesPointSet), style, "main modes");
    }

    void DBViewer::computeMasses(int avRange) {
        for(auto &dbParser : dbParsers) {

            auto &fieldMap = dbParser->getFieldMap();

            for (auto &entry: fieldMap) {
                IN field = *entry.second;
                IN data = field.getSpace().getHostData();

                fix N = field.N;
                fix kMin = field.xMin;

                auto maxInfo = Utils::GetMax(data);

                IN ω = entry.first;
                fix idx = maxInfo.second;
                fix Δk = field.xMax - field.xMin;

                auto k_avg = 0.0;
                auto norm = 0.0;
                int i = -avRange;
                for (; i <= avRange; ++i) {
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

    void DBViewer::drawTable(int specialIndex) {

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

        fix unit = Constants::π;
        auto &fieldMap = dbParsers[0]->getFieldMap();
        int i=0;
        for (auto &entry : fieldMap)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            fix ω = entry.first;
            ImGui::TextUnformatted(unit(ω, 4).c_str());

            ImGui::TableSetColumnIndex(1);
            fix maxData = maxValues[i++];
            ImGui::TextUnformatted(ToStr(maxData.first, 5).c_str());

            ImGui::TableSetColumnIndex(2);
            fix idx = maxData.second;
            IN field = *entry.second;
            fix kMax = field.xMax;
            fix kMin = field.xMin;
            fix Δk = kMax-kMin;
            fix k = Δk*(Real)idx/(Real)field.N - kMin;
            ImGui::TextUnformatted(unit(k, 4).c_str());

            ImGui::TableSetColumnIndex(3);
            fix m² = ω*ω - k*k;
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

    bool DBViewer::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {
        if( key==Core::Key_LEFT_SHIFT  ) shiftKey = state;


        if( key==Core::Key_F5 && state==Core::Press ){
            reloadData();
            return true;        }
        else if( key==Core::Key_ESCAPE && state==Core::Press ){
            topRow.removeWindow(DummyPtr(fullParticularHistoryDisplay));
            return true;
        }

        return WindowRow::notifyKeyboard(key, state, modKeys);
    }

    bool DBViewer::notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) {
        static Timer timer;
        auto elTime = timer.getElTime_msec();
        if(button==Core::MouseButton_LEFT){
            if(state==Core::Press) timer.reset();
            else if(state==Core::Release && elTime < 200 && keys.Mod_Ctrl) {
                loadDataUnderMouse();
                return true;
            }
        }

        return WindowRow::notifyMouseButton(button, state, keys);
    }

    void DBViewer::reloadData() {
        if(!fullFields.empty()) NOT_IMPLEMENTED_CLASS_METHOD;

        for (auto &dbParser: dbParsers) {
            auto fullField = dbParser->buildFullField();
            auto dbRootFolder = StrUtils::ReplaceAll(dbParser->getRootDatabaseFolder(), "./", "");

            allDataDisplay.addFunction(fullField, dbRootFolder);
            fullFields.emplace_back(fullField);
        }

        computeMasses(10);
    }

    void DBViewer::loadDataUnderMouse() {
        auto &fSet = dbParsers[0]->getFileSet();

        auto index = index_XHair-1;
        if(index<0 || index>=fSet.size()) return;

        std::vector files(fSet.begin(), fSet.end());
        auto filename = files[index].second;

        StrUtils::ReplaceLastOccurrence(filename, ".dft.simsnap", ".oscb");
        StrUtils::ReplaceLastOccurrence(filename, "snapshots/", "./");

        Log::Info() << "Find history" << Log::Flush;
        auto fieldHistory = fullHistoriesMap[filename];

        if(fieldHistory == nullptr) {
            Log::Info() << "Not found. Loading history" << Log::Flush;
            fieldHistory = HistoryFileLoader::Load(filename);
            fullHistoriesMap[filename] = fieldHistory;
        }
        Log::Info() << "Set function" << Log::Flush;
        StrUtils::ReplaceAll(filename, ".oscb", "");
        auto omegaStr = StrUtils::Split(filename, " ").back();
        omegaStr = StrUtils::Split(omegaStr, "=")[1];
        ;
        fullParticularHistoryDisplay.addFunction(fieldHistory, Str("ω=") + omegaStr);

        Log::Info() << "Add window" << Log::Flush;
        topRow.addWindow(DummyPtr(fullParticularHistoryDisplay));

        Log::Info() << "Done\n" << Log::Flush;
    }

    bool DBViewer::notifyMouseMotion(int x, int y) {
        if( shiftKey == Core::Press ){
            loadDataUnderMouse();
            return true;
        };

        return WindowRow::notifyMouseMotion(x, y);
    }

}