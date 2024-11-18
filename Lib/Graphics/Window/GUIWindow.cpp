//
// Created by joao on 8/8/22.
//

#include "GUIWindow.h"

#include <utility>

#include "3rdParty/ImGui.h"
#include "WindowStyles.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "Core/Backend/BackendManager.h"

#include "Math/SlabMath.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "StudioSlab.h"

namespace Slab::Graphics {

    GUIWindow::GUIWindow(Config config) : SlabWindow(std::move(config)) {
        setClear(false);
        setDecorate(false);

        auto &gui_module = Slab::GetModule<ImGuiModule>("ImGui");
        gui_context = DynamicPointerCast<SlabImGuiContext>(gui_module.createContext(parent_system_window));

        if(gui_context == nullptr) throw Exception("Failed to get GUIContext.");

        addResponder(gui_context);
    }


    void GUIWindow::addVolatileStat(const Str &stat, const Color color) {
        stats.emplace_back(stat, color);
    }

    void GUIWindow::draw() {
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (-1)");
        SlabWindow::draw();
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (0)");

        begin();
        gui_context->AddDrawCall([this]() {
            auto vp = getViewport();
            const auto w_ = (float) vp.width(),
                    h_ = (float) vp.height();
            const auto x_ = (float) vp.xMin,
                    y_ = (float) vp.yMin;

            ImGui::SetWindowPos(ImVec2{x_, y_});
            ImGui::SetWindowSize(ImVec2{w_, h_});

            auto flags = ImGuiTreeNodeFlags_DefaultOpen;
            if (!stats.empty() && ImGui::CollapsingHeader("Stats", flags)) {
                for (const auto &stat: stats) {
                    const auto text = stat.first;

                    if (text == "<\\br>") {
                        ImGui::Separator();
                        continue;
                    }

                    const auto c = stat.second;
                    if (c.r < 0 || c.g < 0 || c.b < 0 || c.a < 0) {
                        ImGui::Text("%s", text.c_str());
                    } else {
                        const auto color = ImVec4(c.r, c.g, c.b, c.a);
                        ImGui::TextColored(color, text.c_str(), nullptr);
                    }

                }
            }

            auto allDataEntries = Math::EnumerateAllData();
            if (!allDataEntries.empty() && ImGui::CollapsingHeader("Data")) {
                if (ImGui::BeginTable("DataTable", 1)) {
                    ImGui::TableSetupColumn("Name");
                    // ImGui::TableSetupColumn("Id");
                    ImGui::TableHeadersRow();

                    for (const auto &entry: allDataEntries) {
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

            stats.clear();
        });

        end();

        gui_context->NewFrame();
        gui_context->Render();
    }

    void GUIWindow::begin() const {
        gui_context->AddDrawCall([this]() {
            bool closable = false;

            ImGui::Begin("Stats", &closable,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
        });
    }

    void GUIWindow::end() const {
        gui_context->AddDrawCall([]() { ImGui::End(); });
    }

    void GUIWindow::AddExternalDraw(const DrawCall& draw) { gui_context->AddDrawCall(draw); }

    Pointer<SlabImGuiContext> GUIWindow::GetGUIContext() {
        return gui_context;
    }

}