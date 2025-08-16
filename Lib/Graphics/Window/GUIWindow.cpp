//
// Created by joao on 8/8/22.
//

#include "GUIWindow.h"

#include <utility>

#include "imgui_internal.h"
#include "3rdParty/ImGui.h"
#include "../../Core/Controller/InterfaceManager.h"
#include "Core/Backend/BackendManager.h"

#include "Math/SlabMath.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "StudioSlab.h"

namespace Slab::Graphics {

    FGUIWindow::FGUIWindow(const FSlabWindowConfig& Config)
    : FSlabWindow(Config)
    , WindowId(Config.Title.empty() ? GetUniqueName() : Config.Title)
    {
        SetClear(false);
        SetDecorate(false);
    }


    void FGUIWindow::AddVolatileStat(const Str &stat, const FColor color) {
        Stats.emplace_back(stat, color);
    }

    void FGUIWindow::RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow)
    {
        FSlabWindow::RegisterDeferredDrawCalls(PlatformWindow);

        Begin();

        auto Flags = ImGuiTreeNodeFlags_DefaultOpen;
        if (!Stats.empty() && ImGui::CollapsingHeader("Stats", Flags)) {
            for (const auto &stat: Stats) {
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

        if (auto AllDataEntries = Math::EnumerateAllData(); !AllDataEntries.empty() && ImGui::CollapsingHeader("Data")) {
            if (ImGui::BeginTable("DataTable", 2, ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Name");
                // ImGui::TableSetupColumn("Id");
                ImGui::TableHeadersRow();

                for (IN [Name, Type]: AllDataEntries) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", Type.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", Name.c_str());
                }

                ImGui::EndTable();
            }
        }


        if (const auto AllInterfaces = Core::FInterfaceManager::GetInstance().GetInterfaces();
            !AllInterfaces.empty() && ImGui::CollapsingHeader("Interfaces")) {

            for (auto &Interface: AllInterfaces) {
                fix generalDescription = Interface->GetGeneralDescription();

                fix text = Interface->GetName() +
                           (!generalDescription.empty() ? "(" + generalDescription + ")" : "");


                if (ImGui::TreeNode(text.c_str())) {
                    for (auto &param: Interface->GetParameters()) {
                        fix descr = param->getDescription();
                        fix longName = param->getCommandLineArgumentName(true);
                        fix shortName = param->getCommandLineArgumentName(false);

                        auto name = Str("-");
                        if (longName.size() > 1) {
                            name += "-" + longName;
                            if (shortName.size() == 1) name += ", -" + shortName;
                        } else name += longName;

                        ImGui::Text("%s", name.c_str());

                        ImGui::Text("\tValue: %s", param->ValueToString().c_str());

                        if (!descr.empty())
                            ImGui::Text("\tDescr.: %s", descr.c_str());

                    }

                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }
        }

        Stats.clear();

        End();
    }

    void FGUIWindow::Begin() const {
        if (GImGui == nullptr) return;
        if (!GImGui->WithinFrameScope) return;

        fix WinSize = ImVec2(static_cast<float>(this->GetWidth()), static_cast<float>(this->GetHeight()));
        fix WinPos  = ImVec2(static_cast<float>(this->Get_x()), static_cast<float>(this->Get_y()));
        constexpr auto WinFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;

        ImGui::SetNextWindowSize(WinSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(WinPos, ImGuiCond_Always);
        ImGui::Begin(WindowId.c_str(), nullptr, WinFlags);

        auto Flags = ImGuiChildFlags_Border
                   // | ImGuiChildFlags_FrameStyle
                   | ImGuiChildFlags_AutoResizeX
                   | ImGuiChildFlags_AutoResizeY;
        ImGui::BeginChild("##LeftPane", ImVec2{this->GetWidth() - 40.0f,0}, Flags);
    }

    void FGUIWindow::End() const
    {
        if (GImGui == nullptr) return;
        if (!GImGui->WithinFrameScope) return;

        ImGui::EndChild();
        ImGui::End();
    }

    void FGUIWindow::AddExternalDraw(const FDrawCall& Draw) const
    {
        if (GImGui == nullptr) return;
        if (!GImGui->WithinFrameScope) return;

        this->Begin();
        Draw();
        this->End();
    }
}