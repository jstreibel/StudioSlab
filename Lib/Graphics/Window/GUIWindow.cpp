//
// Created by joao on 8/8/22.
//

#include "GUIWindow.h"

#include "imgui.h"
#include "WindowStyles.h"
#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Backend/BackendManager.h"


namespace Slab::Graphics {

    GUIWindow::GUIWindow() : Window() {
        setClear(false);
        setDecorate(false);

        Core::BackendManager::LoadModule(Core::ImGui);
    }


    void GUIWindow::addVolatileStat(const Str &stat, const Color color) {
        stats.emplace_back(stat, color);
    }

    void GUIWindow::draw() {
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (-1)");
        Window::draw();
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (0)");

        auto displayHeight = ImGui::GetIO().DisplaySize.y;

        auto vp = getViewport();
        // const float w_ = float(getw()) - (float)2 * Graphics::hPadding,
        //             h_ = float(geth()) - (float)2 * Graphics::vPadding;
        // const float x_ = getx() + Graphics::hPadding,
        //             y_ = displayHeight - (h_ + gety() + Graphics::vPadding);
        const float w_ = vp.width(),
                h_ = vp.height();
        const float x_ = vp.xMin,
                y_ = displayHeight - vp.yMax;

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

        auto allInterfaces = Core::InterfaceManager::getInstance().getInterfaces();
        if (!allInterfaces.empty() && ImGui::CollapsingHeader("Interfaces")) {

            for (auto &interface: allInterfaces) {
                fix generalDescription = interface->getGeneralDescription();

                fix text = interface->getName() +
                           (!generalDescription.empty() ? "(" + generalDescription + ")" : "");


                if (ImGui::TreeNode(text.c_str())) {
                    for (auto &param: interface->getParameters()) {
                        fix descr = param->getDescription();
                        fix longName = param->getCLName(true);
                        fix shortName = param->getCLName(false);

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

        ImGui::Begin("Stats", &closable,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
    }

    void GUIWindow::end() const {
        ImGui::End();
    }

}