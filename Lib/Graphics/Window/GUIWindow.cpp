//
// Created by joao on 8/8/22.
//

#include "GUIWindow.h"

#include <utility>

#include "3rdParty/ImGui.h"
#include "WindowStyles.h"
#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"
#include "Core/Backend/BackendManager.h"

#include "Math/SlabMath.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "StudioSlab.h"

constexpr bool MAKE_LOCAL_CONTEXT = false;

namespace Slab::Graphics {

    FGUIWindow::FGUIWindow(FConfig config) : FSlabWindow(std::move(config)) {
        SetClear(false);
        SetDecorate(false);

        if constexpr (MAKE_LOCAL_CONTEXT)
        {
            auto &GuiModule = Slab::GetModule<FImGuiModule>("ImGui");
            GuiContext = DynamicPointerCast<FImGuiContext>(GuiModule.CreateContext(w_ParentPlatformWindow));

            if(GuiContext == nullptr) throw Exception("Failed to get GUIContext.");

            AddResponder(GuiContext);
        }
        else
        {
            auto Context = GetGraphicsBackend()->GetMainSystemWindow()->GetGUIContext();
            if(Context == nullptr) throw Exception("Failed to get GUIContext.");

            GuiContext = DynamicPointerCast<FImGuiContext>(Context);
        }
    }


    void FGUIWindow::AddVolatileStat(const Str &stat, const FColor color) {
        Stats.emplace_back(stat, color);
    }

    void FGUIWindow::ImmediateDraw() {
        FSlabWindow::ImmediateDraw();

        if constexpr (MAKE_LOCAL_CONTEXT)
        {
            GuiContext->NewFrame();
            GuiContext->Render();
        }
    }

    void FGUIWindow::RegisterDeferredDrawCalls()
    {
        FSlabWindow::RegisterDeferredDrawCalls();

        Begin();
        GuiContext->AddDrawCall([this]() {
            auto Viewport = GetViewport();
            const auto VpWidth = (float) Viewport.GetWidth(),
                    VpHeight = (float) Viewport.GetHeight();
            const auto Vp_x = (float) Viewport.xMin,
                    Vp_y = (float) Viewport.yMin;

            ImGui::SetWindowPos(ImVec2{Vp_x, Vp_y});
            ImGui::SetWindowSize(ImVec2{VpWidth, VpHeight});

            auto flags = ImGuiTreeNodeFlags_DefaultOpen;
            if (!Stats.empty() && ImGui::CollapsingHeader("Stats", flags)) {
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


            if (auto AllInterfaces = Core::FCommandLineInterfaceManager::getInstance().getInterfaces();
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
        GuiContext->AddDrawCall([this] {
            bool closable = false;

            // ImGui::SetNextWindowPos(ImVec2(this->Get_x(), this->Get_y()), ImGuiCond_Always);
            // ImGui::SetNextWindowSize(ImVec2(this->GetWidth(), this->GetHeight()), ImGuiCond_Always);

            ImGui::Begin("Stats", &closable,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
        });
    }

    void FGUIWindow::End() const {
        GuiContext->AddDrawCall([]() { ImGui::End(); });
    }

    void FGUIWindow::AddExternalDraw(const FDrawCall& draw) const
    {
        this->Begin();
        GuiContext->AddDrawCall(draw);
        this->End();
    }

    Pointer<FImGuiContext> FGUIWindow::GetGUIContext() {
        return GuiContext;
    }

}