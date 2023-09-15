//
// Created by joao on 8/8/22.
//

#include "GUIWindow.h"

#include "imgui.h"
#include "Core/Graphics/Styles/WindowStyles.h"
#include "Core/Controller/Interface/InterfaceManager.h"


GUIWindow::GUIWindow() : Window( ) {
    setClear(false);
    setDecorate(false);
}


void GUIWindow::addVolatileStat(const Str &stat, const Styles::Color color)
{
    stats.emplace_back(stat, color);
}

void GUIWindow::draw() {
    Window::draw();

    auto  displayHeight = ImGui::GetIO().DisplaySize.y;

    const float w_ = float(getw()) - (float)2 * Core::Graphics::hPadding,
                h_ = float(geth()) - (float)2 * Core::Graphics::vPadding;
    const float x_ = getx() + Core::Graphics::hPadding,
                y_ = displayHeight - (h_ + gety() + Core::Graphics::vPadding);

    const float hSpacing = 20.0f;

    begin();

    ImGui::SetWindowPos( ImVec2{x_, y_});
    ImGui::SetWindowSize(ImVec2{w_, h_});

    if(ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen)) {

        auto i = 0;
        for (auto stat: stats) {
            const auto c = stat.second;
            const auto text = stat.first;
            const auto color = ImVec4(c.r, c.g, c.b, c.a);

            if (text == "<\\br>") {
                ImGui::Separator();
                i = 0;
                continue;
            }

            if (i++ % 2 && w_ > 500) ImGui::SameLine(getw() / 2. - hSpacing);

            ImGui::TextColored(color, text.c_str(), nullptr);
        }
    }

    if(ImGui::CollapsingHeader("Interfaces")) {

        for(auto &interface : InterfaceManager::getInstance().getInterfaces()) {
            fix generalDescription = interface->getGeneralDescription();

            fix text = interface->getName() + (!generalDescription.empty() ? "("+generalDescription+")" : "");


            if (ImGui::TreeNode(text.c_str()))
            {
                for(auto &param : interface->getParameters()){
                    fix descr = param->getDescription();
                    fix longName = param->getCLName(true);
                    fix shortName = param->getCLName(false);

                    auto name = Str("-");
                    if(longName.size() > 1) {
                        name += "-" + longName;
                        if(shortName.size() == 1) name += ", -" + shortName;
                    }
                    else name += longName;

                    ImGui::Text("%s", name.c_str());

                    ImGui::Text("\tValue: %s", param->valueToString().c_str());

                    if(!descr.empty())
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
    bool closable=false;

    ImGui::Begin("Stats", &closable,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
}

void GUIWindow::end() const {
    ImGui::End();
}

