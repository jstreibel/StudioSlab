//
// Created by joao on 21/09/23.
//

#include "DBViewer.h"
#include "Math/Constants.h"

#include <utility>
#include "KGDispersionRelation.h"
#include "Maps/FunctionRenderer.h"

#include "3rdParty/imgui/imgui.h"

namespace Modes::DatabaseViewer {

    DBViewer::DBViewer(DBParser::Ptr _dbParser)
    : dbParser(std::move(_dbParser))
    , guiWindow()
    , allDataDisplay("All data", 0.0, 0.5)
    {
        this->addWindow(DummyPtr(guiWindow));
        this->addWindow(DummyPtr(allDataDisplay), .8);

        fullField = dbParser->buildFullField();
        allDataDisplay.setVerticalUnit(Constants::π);
        allDataDisplay.setHorizontalUnit(Constants::π);
        allDataDisplay.setFunction(fullField);
        allDataDisplay.setColorMap(Styles::ColorMaps["blues"].inverse());

        drawKGDispersion();

        auto &fieldMap = dbParser->getFieldMap();
        for (auto &entry : fieldMap){
            auto &field = *entry.second;
            fix kMax = field.getSpace().getHostData().max();
            max_kValues.emplace_back(kMax);
        }
    }

    void DBViewer::draw() {

        guiWindow.begin();

        if(ImGui::CollapsingHeader("Database")) {
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

            if (ImGui::BeginTable("table1", 3, flags))
            {
                Str paramName = dbParser->getCriticalParameter();

                ImGui::TableSetupColumn((paramName + " (x axis)").c_str());
                ImGui::TableSetupColumn("Aₘₐₓ");
                ImGui::TableSetupColumn("...");
                ImGui::TableHeadersRow();

                fix unit = Constants::π;
                auto &fieldMap = dbParser->getFieldMap();
                int i=0;
                for (auto &entry : fieldMap)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(unit(entry.first, 4).c_str());

                    ImGui::TableSetColumnIndex(1);
                    fix kMax = max_kValues[i++];
                    ImGui::TextUnformatted(ToStr(kMax, 5).c_str());

                    if(false)
                    {
                        ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.35f));
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                    }

                }
                ImGui::EndTable();
            }
        }

        if(ImGui::CollapsingHeader("Klein-Gordon dispersion relation")){
            static bool isVisible = true;

            auto mass = (float)KG_mass;
            if(ImGui::SliderFloat("mass", &mass, 0.0, 10.0)) {
                KG_mass = mass;
                isVisible = true;
                drawKGDispersion();
            }

            if(ImGui::Checkbox("Visible", &isVisible)){
                if(isVisible) drawKGDispersion();
                else allDataDisplay.clearPointSets();
            }
        }
        guiWindow.end();

        WindowRow::draw();
    }

    void DBViewer::drawKGDispersion() {
        Real mass = KG_mass;
        auto KGRelation = RtoR::FunctionRenderer::toPointSet(
                RtoR::KGDispersionRelation(mass, RtoR::KGDispersionRelation::k_AsFunctionOf_ω),
                0,
                fullField->getDomain().xMax,
                5000);
        auto style = Styles::GetCurrent()->funcPlotStyles[5];
        style.thickness = 3;
        style.filled = false;
        allDataDisplay.clearPointSets();
        allDataDisplay.addPointSet(KGRelation, style, Str("ω²-k²-m²=0   (Klein-Gordon with m=") + ToStr(mass) + ")");
    }
}