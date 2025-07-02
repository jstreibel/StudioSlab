//
// Created by joao on 8/8/22.
//

#include "GUIWindow.h"

#include <utility>

#include "3rdParty/ImGui.h"
#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"
#include "Core/Backend/BackendManager.h"

#include "Math/SlabMath.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "StudioSlab.h"

#define GLOBAL_IMGUI_CONTEXT Slab::DynamicPointerCast<Slab::Graphics::FImGuiContext>(Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow()->GetGUIContext())

namespace Slab::Graphics {

    FGUIWindow::FGUIWindow(FSlabWindowConfig config, const FImGuiWindowContext& WindowContext)
    : FSlabWindow(std::move(config))
    , WindowContext(WindowContext)
    {
        if (this->WindowContext.Context == nullptr) this->WindowContext.Context = GLOBAL_IMGUI_CONTEXT;
        if (this->WindowContext.WindowId == "")     this->WindowContext.WindowId = GetUniqueName();

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
        WindowContext.Context->AddDrawCall([this] {
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

            auto allDataEntries = Math::EnumerateAllData();
            if (!allDataEntries.empty() && ImGui::CollapsingHeader("Data")) {
                if (ImGui::BeginTable("DataTable", 2, ImGuiTableFlags_Resizable)) {
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupColumn("Name");
                    // ImGui::TableSetupColumn("Id");
                    ImGui::TableHeadersRow();

                    for (const auto &entry: allDataEntries) {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", entry.type.c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", entry.name.c_str());
                    }

                    ImGui::EndTable();
                }
            }


            if (const auto AllInterfaces = Core::FCommandLineInterfaceManager::getInstance().getInterfaces();
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
        });
        End();
    }

    void FGUIWindow::Begin() const {
        WindowContext.Context->AddDrawCall([this]
        {
            ImGui::Begin(WindowContext.WindowId.c_str());

            ImGui::BeginChild("##LeftPane",
                ImVec2(this->GetWidth(), this->GetHeight()),
                ImGuiChildFlags_FrameStyle);
        });
    }

    void FGUIWindow::End() const
    {
        WindowContext.Context->AddDrawCall([]
        {
            ImGui::EndChild();
            ImGui::End();
        });
    }

    void FGUIWindow::AddExternalDraw(const FDrawCall& Draw) const
    {
        this->Begin();
        WindowContext.Context->AddDrawCall(Draw);
        this->End();
    }

    FImGuiWindowContext FGUIWindow::GetGUIWindowContext() {
        return WindowContext;
    }

}