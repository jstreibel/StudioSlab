//
// Created by joao on 8/14/25.
//

#include "LabWindowManager.h"

#include <utility>

#include "imgui.h"
#include "StudioConfig.h"
#include "StudioSlab.h"
#include "Core/SlabCore.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/ImGui/ImGuiWindowManager.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Math/SlabMath.h"
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

void StudioWindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow>& SlabWindow)
{
    AddSlabWindow(SlabWindow, false);
}

void StudioWindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow>& Window, bool hidden)
{
    AddResponder(Window);

    SlabWindows.emplace_back(Window);

    NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
}

void ShowJobs();
void ShowManagedData(StudioWindowManager& Self);
void ShowMainMenuEntries(const Slab::Graphics::FPlatformWindow &PlatformWindow, Slab::Graphics::FImGuiContext &ImGuiContext);

bool StudioWindowManager::NotifyRender(const Slab::Graphics::FPlatformWindow& PlatformWindow)
{
    ImGuiContext->NewFrame();
    ImGuiContext->SetupOptionalMenuItems();

    // Local GUI logic
    {
        fix MenuHeight = Slab::Graphics::WindowStyle::GlobalMenuHeight;
        ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(MenuHeight)));
        ImGui::SetNextWindowSize(ImVec2(StudioConfig::SidePaneWidth, static_cast<float>(HeightSysWin - MenuHeight)), ImGuiCond_Appearing);
        constexpr auto WindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin(StudioConfig::SidePaneId, nullptr, WindowFlags))
        {
            const auto TaskManager = Slab::Core::GetModule<Slab::Core::MTaskManager>("TaskManager");

            ShowJobs();
            ShowManagedData(*this);

            if (fix WindowWidth = static_cast<int>(ImGui::GetWindowWidth()); SidePaneWidth != WindowWidth)
            {
                SidePaneWidth = WindowWidth;
                NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
            }

            ImGui::End();
        }

        ShowMainMenuEntries(PlatformWindow, *ImGuiContext);
    }

    for (const auto& Window : SlabWindows) if (Window->WantsClose()){ RemoveResponder(Window); std::erase(SlabWindows, Window); }

    FWindowManager::NotifyRender(PlatformWindow);

    ImGuiContext->Render();

    return true;
}


bool StudioWindowManager::NotifySystemWindowReshape(int w, int h)
{
    WidthSysWin = w;
    HeightSysWin = h;

    /*
    if (SlabWindow != nullptr)
    {
        fix MenuHeight = Slab::Graphics::WindowStyle::GlobalMenuHeight;
        fix Gaps = Slab::Graphics::WindowStyle::TilingGapSize;

        SlabWindow->NotifyReshape(WidthSysWin-SidePaneWidth-2*Gaps, HeightSysWin-MenuHeight-2*Gaps);
        SlabWindow->Set_x(SidePaneWidth + Gaps);
        SlabWindow->Set_y(MenuHeight + Gaps);
    }
    */

    return FWindowManager::NotifySystemWindowReshape(w, h);
}

Slab::TPointer<Slab::Graphics::FImGuiContext> StudioWindowManager::GetImGuiContext()
{
    return ImGuiContext;
}

void ShowJobs()
{
    const auto TaskManager = Slab::Core::GetModule<Slab::Core::MTaskManager>("TaskManager");

    // SHOW JOBS **********************************************************************
    if (auto Jobs = TaskManager->GetAllJobs(); !Jobs.empty())
    {
        ImGui::SeparatorText("Tasks");
        for (auto & Job : Jobs)
        {
            auto &[Task, JobThread] = Job;
            ImGui::Text("%s: ", Task->GetName().c_str());
            ImGui::SameLine();

            switch (Task->GetStatus())
            {
            case Slab::Core::TaskRunning:
                ImGui::TextColored(ImVec4(0,   0,   1, 1), "Running");
                break;
            case Slab::Core::TaskAborted:
                ImGui::TextColored(ImVec4(.8f, .4f, 0, 1), "Aborted");
                break;
            case Slab::Core::TaskError:
                ImGui::TextColored(ImVec4(1,   0,   1, 1), "Error");
                break;
            case Slab::Core::TaskSuccess:
                ImGui::TextColored(ImVec4(0,   1,   0, 1), "Success");
                ImGui::SameLine();
                if (ImGui::Button("X"))
                {
                    Task->Release();
                    TaskManager->ClearJob(Job);
                }
                break;
            case Slab::Core::TaskNotInitialized:
                ImGui::TextColored(ImVec4(1,   1,   0, 1), "Running");
                break;
            default:
                ImGui::TextColored(ImVec4(1,   0,   0, 1), "Unknown state");
            }

            if (Task->GetStatus() == Slab::Core::TaskRunning)
            {
                try
                {
                    const auto NumericTask = dynamic_cast<Slab::Math::FNumericTask*>(Task.get());
                    fix Progress = NumericTask->GetProgress();
                    ImGui::SameLine();
                    ImGui::ProgressBar(Progress);

                } catch (std::bad_cast &)
                {

                }
            }

        }
    }
}

void ShowManagedData(StudioWindowManager& Self)
{
    if (const auto AllManagedData = Slab::Math::FDataManager::GetDataList(); !AllManagedData.empty())
    {
        ImGui::SeparatorText("Data");
        static int SelectedRow = -1;

        if (ImGui::BeginTable("Data Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Size (MiB)", ImGuiTableColumnFlags_NoHide);
            ImGui::TableHeadersRow();

            static Slab::TPointer<Slab::Math::Data> SelectedData;
            int Row = 0;
            for (const auto& Data : AllManagedData)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                fix UniqueName = Data->get_data_name() + " (" + Slab::ToStr(Data->get_id()) + ")";
                if (bool RowSelected = SelectedRow == Row;
                    ImGui::Selectable(UniqueName.c_str(), RowSelected,
                        ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
                {
                    SelectedRow = Row; // mark which row is selected
                    SelectedData = Data;
                    ImGui::OpenPopup("Data Options##DataOptions");
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", Data->get_data_type().c_str());

                auto Size = Data->get_data_size_MiB();
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.3fMiB", Size);

                ++Row;
            }
            if (ImGui::BeginPopup("Data Options##DataOptions"))
            {
                ImGui::Text("Data Options");
                ImGui::Separator();
                if (ImGui::Selectable("View##DataOptions"))
                {
                    using NumericFunction = Slab::Math::R2toR::FNumericFunction;
                    auto Cast = [](auto Obj) {return Slab::DynamicPointerCast<NumericFunction>(std::move(Obj));};
                    using Plotter = Slab::Graphics::FPlotter;
                    Slab::TPointer<NumericFunction> Function;
                    try { Function = Cast(SelectedData); } catch (std::bad_cast &){ }

                    auto PlotWindow = Slab::New<Slab::Graphics::FPlot2DWindow>(SelectedData->get_data_name());
                    Plotter::AddR2toRFunction(PlotWindow, Function, SelectedData->get_data_name());

                    auto Window = Slab::New<Slab::Graphics::FSlabWindow_ImGuiWrapper>(PlotWindow, Self.GetImGuiContext());

                    Self.AddSlabWindow(Window, false);
                }

                if (ImGui::Selectable("Delete##DataOptions"))
                {
                    Slab::Math::FDataManager::Delete(SelectedData);
                    SelectedData = nullptr;
                }
                ImGui::EndPopup();
            }
            ImGui::EndTable();
        }

    }

    if (const auto AllDataRegistries = Slab::Math::EnumerateAllData(); !AllDataRegistries.empty() && ImGui::CollapsingHeader("Data Registry"))
    {
        if (ImGui::Button("Prune Data")) Slab::Math::FDataRegistry::Prune();

        else
        {
            static int SelectedRow = -1;

            if (ImGui::BeginTable("Volatile Data", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_NoHide);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_NoHide);


                ImGui::TableHeadersRow();

                static Slab::Math::FDataRegistry::EntryDescription SelectedRegistry{""};
                int Row = 0;
                for (const auto& RegistryDescription : AllDataRegistries)
                {
                    fix Name = RegistryDescription.Name;

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    auto DataWrap = Slab::Math::FDataRegistry::GetData(Name);

                    if (auto Data = DataWrap.GetData(); Data == nullptr)
                    {
                        ImGui::TextColored(ImVec4{0.75f, 0, 0, 1}, "Deleted");
                    }
                    else
                    {
                        ImGui::TextColored(ImVec4{0, 0.75, 0, 1}, "Active");
                    }


                    ImGui::TableSetColumnIndex(1);
                    if (bool RowSelected = SelectedRow == Row;
                        ImGui::Selectable(Name.c_str(), RowSelected,
                            ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
                    {
                        SelectedRow = Row; // mark which row is selected
                        SelectedRegistry = RegistryDescription;
                        ImGui::OpenPopup("Registry Options##DataOptions");
                    }

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", RegistryDescription.Type.c_str());

                    ++Row;
                }
                if (ImGui::BeginPopup("Registry Options##DataOptions"))
                {
                    ImGui::Text("Registry Options");
                    ImGui::Separator();

                    fix InstanceExists = Slab::Math::FDataManager::Contains(SelectedRegistry);
                    fix Flags = InstanceExists ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None;
                    if (ImGui::Selectable("Manage##DataOptions", false, Flags))
                    {
                        fix DataWrap = Slab::Math::FDataRegistry::GetData(SelectedRegistry.Name);
                        if (fix Data = DataWrap.GetData(); Data != nullptr)
                            Slab::Math::FDataManager::AddData(Data);
                    }
                    ImGui::EndPopup();
                }
                ImGui::EndTable();
            }
        }
    }
}

void ShowMainMenuEntries(const Slab::Graphics::FPlatformWindow &PlatformWindow, Slab::Graphics::FImGuiContext &ImGuiContext)
{
    fix ItemLocation = Slab::Graphics::MainMenuLocation{"System"};
    auto Entry = Slab::Graphics::MainMenuLeafEntry{"Exit", "Alt+F4"};
    auto Action = [&PlatformWindow](const Slab::Str &Item){
        if (Item == "Exit")
            const_cast<Slab::Graphics::FPlatformWindow*>(&PlatformWindow)->SignalClose();
    };
    fix Item = Slab::Graphics::MainMenuItem{ItemLocation, {Entry}, Action};
    ImGuiContext.AddMainMenuItem(Item);
}