//
// Created by joao on 8/8/22.
//

#include "GUIWindow.h"

#include "3rdParty/ImGui.h"
#include "WindowStyles.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "Core/Backend/BackendManager.h"

#include "Math/SlabMath.h"


namespace Slab::Graphics {

    GUIWindow::GUIWindow(Config config) : SlabWindow(config) {
        setClear(false);
        setDecorate(false);

        Core::BackendManager::LoadModule("ImGui");
    }


    void GUIWindow::addVolatileStat(const Str &stat, const Color color) {
        stats.emplace_back(stat, color);
    }

    void GUIWindow::draw() {
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (-1)");
        SlabWindow::draw();
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (0)");

        auto displayHeight = ImGui::GetIO().DisplaySize.y;

        auto vp = getViewport();
        // const float w_ = float(getw()) - (float)2 * Graphics::hPadding,
        //             h_ = float(geth()) - (float)2 * Graphics::vPadding;
        // const float x_ = getx() + Graphics::hPadding,
        //             y_ = displayHeight - (h_ + gety() + Graphics::vPadding);
        const auto w_ = (float)vp.width(),
                   h_ = (float)vp.height();
        const auto x_ = (float)vp.xMin,
                   y_ = (float)vp.yMin;

        const float hSpacing = 20.0f;

        begin();

        ImGui::SetWindowPos(ImVec2{x_, y_});
        ImGui::SetWindowSize(ImVec2{w_, h_});

        auto flags = ImGuiTreeNodeFlags_DefaultOpen;
        if (!stats.empty() && ImGui::CollapsingHeader("Stats", flags)) {

            auto i = 0;
            for (const auto &stat: stats) {
                const auto text = stat.first;

                if (text == "<\\br>") {
                    ImGui::Separator();
                    i = 0;
                    continue;
                }

                const auto c = stat.second;
                if(c.r<0 || c.g<0 || c.b<0 || c.a<0) {
                    ImGui::Text("%s", text.c_str());
                } else {
                    const auto color = ImVec4(c.r, c.g, c.b, c.a);
                    ImGui::TextColored(color, text.c_str(), nullptr);
                }

            }
        }

        auto allDataEntries = Math::EnumerateAllData();
        if(!allDataEntries.empty() && ImGui::CollapsingHeader("Data")){
            if(ImGui::BeginTable("DataTable", 1)) {
                ImGui::TableSetupColumn("Name");
                // ImGui::TableSetupColumn("Id");
                ImGui::TableHeadersRow();

                for (const auto& entry: allDataEntries) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    ImGui::Text("%s", entry.c_str());
                }

                ImGui::EndTable();
            }
        }


        auto allInterfaces = Core::CLInterfaceManager::getInstance().getInterfaces();
        if (!allInterfaces.empty() && ImGui::CollapsingHeader("Interfaces")) {

            for (auto &interface: allInterfaces) {
                fix generalDescription = interface->getGeneralDescription();

                fix text = interface->getName() +
                           (!generalDescription.empty() ? "(" + generalDescription + ")" : "");


                if (ImGui::TreeNode(text.c_str())) {
                    for (auto &param: interface->getParameters()) {
                        fix descr = param->getDescription();
                        fix longName = param->getCommandLineArgumentName(true);
                        fix shortName = param->getCommandLineArgumentName(false);

                        auto name = Str("-");
                        if (longName.size() > 1) {
                            name += "-" + longName;
                            if (shortName.size() == 1) name += ", -" + shortName;
                        } else name += longName;

                        ImGui::Text("%s", name.c_str());

                        ImGui::Text("\tValue: %s", param->valueToString().c_str());

                        if (!descr.empty())
                            ImGui::Text("\tDescr.: %s", descr.c_str());

                    }

                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }

        }

        end();

        stats.clear();
    }

    void GUIWindow::begin() const {
        bool closable = false;

        ImGui::Begin(unique("Stats").c_str(), &closable,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
    }

    void GUIWindow::end() const {
        ImGui::End();
    }

}