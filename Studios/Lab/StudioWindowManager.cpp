//
// Created by joao on 8/14/25.
//

#include "StudioWindowManager.h"

#include "imgui.h"
#include "StudioConfig.h"
#include "StudioSlab.h"
#include "Core/SlabCore.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Math/Numerics/NumericTask.h"

#include "Simulation/SimulationManager.h"

StudioWindowManager::StudioWindowManager(): SidePaneWidth(StudioConfig::SidePaneWidth)
{
    auto &ImGuiModule = Slab::GetModule<Slab::Graphics::FImGuiModule>("ImGui");
    fix PlatformWindow = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

    ImGuiContext = Slab::DynamicPointerCast<Slab::Graphics::FImGuiContext>(ImGuiModule.CreateContext(PlatformWindow));
    ImGuiContext->SetManualRender(true);

    SimulationManager = Slab::New<FSimulationManager>(ImGuiContext);

    this->AddResponder(ImGuiContext);
    this->AddResponder(SimulationManager);
}

void StudioWindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow>& Window, bool hidden)
{
    if (SlabWindow != nullptr) RemoveResponder(SlabWindow);

    SlabWindow = Window;

    AddResponder(Window);

    NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
}

bool StudioWindowManager::NotifyRender(const Slab::Graphics::FPlatformWindow& PlatformWindow)
{
    ImGuiContext->NewFrame();

    // Local GUI logic
    {
        fix MenuHeight = Slab::Graphics::WindowStyle::GlobalMenuHeight;
        ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(MenuHeight)));
        ImGui::SetNextWindowSize(ImVec2(StudioConfig::SidePaneWidth, static_cast<float>(HeightSysWin - MenuHeight)), ImGuiCond_Appearing);
        if (ImGui::Begin(StudioConfig::SidePaneId, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
        {
            const auto TaskManager = Slab::Core::GetModule<Slab::Core::MTaskManager>("TaskManager");

            auto Jobs = TaskManager->GetAllJobs();
            if (!Jobs.empty())
            {
                ImGui::SeparatorText("Tasks");
                for (auto & [Task, JobThread] : Jobs)
                {
                    ImGui::Text("%s: ", Task->GetName().c_str());
                    ImGui::SameLine();
                    switch (Task->GetStatus())
                    {
                    case Slab::Core::TaskRunning:        ImGui::TextColored(ImVec4(0,   0,   1, 1), "Running"); break;
                    case Slab::Core::TaskAborted:        ImGui::TextColored(ImVec4(.8f, .4f, 0, 1), "Aborted"); break;
                    case Slab::Core::TaskError:          ImGui::TextColored(ImVec4(1,   0,   1, 1), "Error");   break;
                    case Slab::Core::TaskSuccess:        ImGui::TextColored(ImVec4(0,   1,   0, 1), "Success"); break;
                    case Slab::Core::TaskNotInitialized: ImGui::TextColored(ImVec4(1,   1,   0, 1), "Running"); break;
                    default:
                        ImGui::TextColored(ImVec4(1,   0,   0, 1), "Unknown state");
                    }

                    if (Task->GetStatus() == Slab::Core::TaskRunning)
                    {
                        try
                        {
                            auto NumericTask = dynamic_cast<Slab::Math::NumericTask*>(Task.get());
                            fix Progress = NumericTask->GetProgress();
                            ImGui::SameLine();
                            ImGui::ProgressBar(Progress);

                        } catch (std::bad_cast &Exception)
                        {

                        }
                    }

                }
            }

            if (fix WindowWidth = static_cast<int>(ImGui::GetWindowWidth());
                SidePaneWidth != WindowWidth)
            {
                SidePaneWidth = WindowWidth;
                NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
            }
        }
        ImGui::End();

        // Main menu entries
        {
            {
                auto ItemLocation = Slab::Graphics::MainMenuLocation{"System"};
                auto Entry = Slab::Graphics::MainMenuLeafEntry{"Exit", "Alt+F4"};
                auto Action = [&PlatformWindow](const Slab::Str &Item){
                    if (Item == "Exit")
                        const_cast<Slab::Graphics::FPlatformWindow*>(&PlatformWindow)->SignalClose();
                };
                auto Item = Slab::Graphics::MainMenuItem{ItemLocation, {Entry}, Action};
                ImGuiContext->AddMainMenuItem(Item);
            }
        }
    }

    FWindowManager::NotifyRender(PlatformWindow);

    ImGuiContext->Render();

    return true;
}


bool StudioWindowManager::NotifySystemWindowReshape(int w, int h)
{
    WidthSysWin = w;
    HeightSysWin = h;

    if (SlabWindow != nullptr)
    {
        fix MenuHeight = Slab::Graphics::WindowStyle::GlobalMenuHeight;
        fix Gaps = Slab::Graphics::WindowStyle::TilingGapSize;

        SlabWindow->NotifyReshape(WidthSysWin-SidePaneWidth-2*Gaps, HeightSysWin-MenuHeight-2*Gaps);
        SlabWindow->Set_x(SidePaneWidth + Gaps);
        SlabWindow->Set_y(MenuHeight + Gaps);
    }

    return FWindowManager::NotifySystemWindowReshape(w, h);
}

