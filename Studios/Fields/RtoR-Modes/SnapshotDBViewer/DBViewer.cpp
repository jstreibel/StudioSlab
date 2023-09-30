//
// Created by joao on 21/09/23.
//

#include "DBViewer.h"

#include <utility>
#include "KGDispersionRelation.h"

#include "3rdParty/imgui/imgui.h"
#include "Core/Graphics/Window/WindowContainer/WindowColumn.h"
#include "HistoryFileLoader.h"
#include "Core/Backend/BackendManager.h"

namespace Modes::DatabaseViewer {

    DBViewer::DBViewer(DBParser::Ptr _dbParser)
    : dbParser(std::move(_dbParser))
    , guiWindow()
    , allDataDisplay("All data", 0.0, 0.5)
    , fullParticularHistoryDisplay("Particular data")
    {
        this->addWindow(DummyPtr(guiWindow));

        std::shared_ptr<WindowColumn>winCol(new WindowColumn);


        auto style = Styles::GetCurrent()->funcPlotStyles[2];
        style.primitive = Styles::VerticalLines;
        style.filled = false;
        style.thickness = 1.5;
        massesGraph.addPointSet(DummyPtr(massesReal_pointSet), style, "ℜ[m=√(ω²-k²)]", true);
        style.lineColor = style.lineColor.permute(true);
        massesGraph.addPointSet(DummyPtr(massesImag_pointSet), style, "ℑ[m=√(ω²-k²)]");
        style.lineColor = style.lineColor.permute();
        style.thickness = 3.0;
        massesGraph.addPointSet(DummyPtr(underXHair), style);
        massesGraph.setHorizontalUnit(Constants::π);

        allDataDisplay.setVerticalUnit(Constants::π);
        allDataDisplay.setHorizontalUnit(Constants::π);
        allDataDisplay.setColorMap(Styles::ColorMaps["blues"].inverse());

        topRow.addWindow(DummyPtr(allDataDisplay));
        fullParticularHistoryDisplay.setColorMap(Styles::ColorMaps["BrBG"].inverse());
        // winRow->addWindow(DummyPtr(fullParticularHistoryDisplay));

        winCol->addWindow(DummyPtr(massesGraph));
        winCol->addWindow(DummyPtr(topRow), 0.75);

        addWindow(winCol, WindowRow::Left, .8);

        reloadData();
    }

    void DBViewer::draw() {

        fix ω_XHair = allDataDisplay.getLastXHairPosition().x;
        fix dx = fullField->getSpace().getMetaData().geth(0);
        fix L = fullField->getDomain().getLx();
        fix N = fullField->getN();
        fix ωMin = -1.5*dx+fullField->getDomain().xMin;
        index_XHair =  (int)(N*(ω_XHair-ωMin)/(L+dx));

        guiWindow.begin();

        if(ImGui::CollapsingHeader("Dominant modes")) {
            static bool isVisible = false;

            if(ImGui::Checkbox("Visible##dominant_modes", &isVisible)){
                if(isVisible) drawDominantModes();
                else allDataDisplay.removePointSet(DummyPtr(maxValuesPointSet));
            }

            drawTable(index_XHair);

            // if(index_XHair>=0 && index_XHair<=fullField->getN()){ /*underXHair.addPoint({ω_XHair, });*/ };
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
            if(ImGui::BeginMenu("O HAI")){

                ImGui::MenuItem("Item 1");
                ImGui::MenuItem("Item 2");

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        WindowRow::draw();
    }

    void DBViewer::updateKGDispersion(bool visible) {
        Real mass = KG_mass;
        static Spaces::PointSet::Ptr KGRelation;
        allDataDisplay.removePointSet(KGRelation);

        if(!visible) return;

        KGRelation = RtoR::FunctionRenderer::toPointSet(
                RtoR::KGDispersionRelation(mass, RtoR::KGDispersionRelation::k_AsFunctionOf_ω),
                0,
                fullField->getDomain().xMax,
                5000);
        auto style = Styles::GetCurrent()->funcPlotStyles[5];
        style.thickness = 3;
        style.filled = false;
        allDataDisplay.addPointSet(KGRelation, style, Str("ω²-k²-m²=0   (Klein-Gordon with m=") + ToStr(mass) + ")");
    }

    void DBViewer::drawDominantModes() {
        auto style = Styles::GetCurrent()->funcPlotStyles[0];
        style.primitive = Styles::Point;
        style.thickness = 8;
        allDataDisplay.removePointSet("main modes");
        allDataDisplay.addPointSet(DummyPtr(maxValuesPointSet), style, "main modes");
    }

    void DBViewer::computeMasses(int avRange) {
        auto &fieldMap = dbParser->getFieldMap();

        for (auto &entry : fieldMap){
            IN field = *entry.second;
            IN data = field.getSpace().getHostData();

            fix N = field.N;
            fix kMin = field.xMin;

            auto maxInfo = Utils::GetMax(data);

            IN ω = entry.first;
            fix idx = maxInfo.second;
            fix Δk = field.xMax-field.xMin;

            auto k_avg = 0.0;
            auto norm = 0.0;
            int i=-avRange;
            for(; i <= avRange; ++i){
                int curr_idx = idx+i;
                if(curr_idx<0 || curr_idx>=N) continue;

                auto weight = data[curr_idx];
                auto k = Δk*(Real)(curr_idx)/(Real)N - kMin;

                k_avg += k*weight;
                norm += weight;
            }

            auto k = k_avg/norm;

            maxValues.emplace_back(maxInfo);
            maxValuesPointSet.addPoint({ω, k});

            fix m² = ω*ω - k*k;

            if(m²>=0)
                massesReal_pointSet.addPoint({ω, sqrt(m²)});
            else
                massesImag_pointSet.addPoint({ω, sqrt(-m²)});
        }
    }

    void DBViewer::drawTable(int specialIndex) {

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
        // const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        static ImVec2 outer_size_value = ImVec2(0.0f, TEXT_BASE_HEIGHT * 20);

        fix columns = 4;
        if (!ImGui::BeginTable("table1", columns, flags, outer_size_value)) return;

        Str paramName = dbParser->getCriticalParameter();

        ImGui::TableSetupColumn((paramName + " (x axis)").c_str());
        ImGui::TableSetupColumn("Aₘₐₓ");
        ImGui::TableSetupColumn("k (y axis)");
        ImGui::TableSetupColumn("m² = ω² - k²");
        ImGui::TableSetupScrollFreeze(0, 1);

        ImGui::TableHeadersRow();

        fix unit = Constants::π;
        auto &fieldMap = dbParser->getFieldMap();
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
        if( key==Core::Key_F5 && state==Core::Press ){
            reloadData();
            return true;
        }

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
        fullField = dbParser->buildFullField();
        allDataDisplay.setFunction(fullField);

        computeMasses(2);
    }

    void DBViewer::loadDataUnderMouse() {
        auto &fSet = dbParser->getFileSet();

        auto index = index_XHair-1;
        if(index<0 || index>=fSet.size()) return;

        std::vector files(fSet.begin(), fSet.end());
        auto filename = files[index].second;

        StrUtils::ReplaceLastOccurrence(filename, ".dft.simsnap", ".oscb");
        StrUtils::ReplaceLastOccurrence(filename, ".//./snapshots/", "/");

        auto fieldHistory = fullHistoriesMap[filename];

        if(fieldHistory == nullptr) {
            fieldHistory = HistoryFileLoader::Load(filename);
            fullHistoriesMap[filename] = fieldHistory;
        }

        fullParticularHistoryDisplay.setFunction(fieldHistory);

        topRow.addWindow(DummyPtr(fullParticularHistoryDisplay));
    }

    bool DBViewer::notifyMouseMotion(int x, int y) {
        // if(modKeys.Mod_Ctrl && modKeys.Mod_Alt){
        //     loadDataUnderMouse();
        //     return true;
        // }

        return WindowRow::notifyMouseMotion(x, y);
    }

}