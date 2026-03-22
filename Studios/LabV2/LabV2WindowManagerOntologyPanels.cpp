#include "LabV2WindowManager.h"

#include "Graphics/Plot2D/V2/Artists/BackgroundArtistV2.h"
#include "Graphics/Plot2D/V2/Artists/OntologyGraphArtistV2.h"
#include "Graphics/Plot2D/V2/Plot2DWindowHostV2.h"
#include "imgui.h"

#include <algorithm>
#include <filesystem>

namespace {

    namespace OntologyV2 = Slab::Core::Ontology::V2;

    constexpr auto WindowTitleOntologyGraph = "Ontology Graph";

    [[nodiscard]] auto FindOntologyResourcesDirectoryV2() -> std::filesystem::path {
        const auto suffix = std::filesystem::path("Resources") / "Ontologies";
        auto cursor = std::filesystem::current_path();

        while (true) {
            const auto candidate = cursor / suffix;
            if (std::filesystem::exists(candidate / "studioslab-ontology.schema.json") &&
                std::filesystem::exists(candidate / "global-descent.ontology.json")) {
                return candidate;
            }

            const auto parent = cursor.parent_path();
            if (parent.empty() || parent == cursor) break;
            cursor = parent;
        }

        return {};
    }

    [[nodiscard]] auto BuildOntologyStatusSummaryV2(const auto &state) -> Slab::Str {
        if (!state.bLoaded) return "[Error] Ontology documents are not loaded.";

        return "[Ok] Loaded " +
            Slab::ToStr(state.Bundle.StudyDocuments.size()) + " study documents, " +
            Slab::ToStr(state.Bundle.Diagnostics.ErrorCount()) + " errors, " +
            Slab::ToStr(state.Bundle.Diagnostics.WarningCount()) + " warnings.";
    }

    [[nodiscard]] auto FindStudyIndexByIdV2(const auto &state, const Slab::Str &studyId) -> int {
        for (std::size_t index = 0; index < state.Bundle.StudyDocuments.size(); ++index) {
            if (state.Bundle.StudyDocuments[index].Study.Id == studyId) {
                return static_cast<int>(index);
            }
        }
        return -1;
    }

    auto RebuildOntologyProjectionV2(auto &state) -> void {
        if (!state.bLoaded) {
            state.Projection = {};
            return;
        }

        if (state.SelectedStudyId.empty() && !state.Bundle.StudyDocuments.empty()) {
            state.SelectedStudyId = state.Bundle.StudyDocuments.front().Study.Id;
            state.SelectedStudyIndex = 0;
        }

        state.Projection = OntologyV2::BuildOntologyGraphProjectionV2(
            state.Bundle,
            state.SelectedStudyId,
            state.Filters);
        state.bPendingGraphFit = true;

        const auto selectionVisible = [&]() {
            if (!state.Selection.IsValid()) return false;
            if (state.Selection.Kind == OntologyV2::EOntologyElementKindV2::Node) {
                return state.Projection.FindNode(state.Selection.ElementId) != nullptr;
            }
            if (state.Selection.Kind == OntologyV2::EOntologyElementKindV2::Edge) {
                return state.Projection.FindEdge(state.Selection.ElementId) != nullptr;
            }
            return false;
        }();

        if (!selectionVisible) {
            state.Selection = {};
        }
    }

    auto DrawJsonPayloadBlockV2(const char *label, const crude_json::value &value, const float height = 180.0f) -> void {
        ImGui::SeparatorText(label);
        if (ImGui::BeginChild(label, ImVec2(0.0f, height), true)) {
            const auto dump = value.dump(2);
            ImGui::TextUnformatted(dump.c_str());
        }
        ImGui::EndChild();
    }

} // namespace

auto FLabV2WindowManager::EnsureOntologyWorkspaceData() -> void {
    auto &state = OntologyWorkspaceState;
    if (!state.bDirty && state.bLoaded) return;

    const auto preservedFilters = state.Filters;
    const auto preservedSelection = state.Selection;
    const auto preservedStudyId = state.SelectedStudyId;

    state = {};
    state.Filters = preservedFilters;
    state.Selection = preservedSelection;
    state.SelectedStudyId = preservedStudyId;

    state.ResourceDirectory = FindOntologyResourcesDirectoryV2();
    if (state.ResourceDirectory.empty()) {
        state.Status = "[Error] Could not locate Resources/Ontologies from the current working directory.";
        state.bLoaded = false;
        state.bDirty = false;
        return;
    }

    state.SchemaPath = state.ResourceDirectory / "studioslab-ontology.schema.json";
    state.GlobalPath = state.ResourceDirectory / "global-descent.ontology.json";
    state.StudyPaths = OntologyV2::DiscoverOntologyStudyDocumentsV2(state.ResourceDirectory);
    state.Bundle = OntologyV2::LoadOntologyGraphBundleV2(state.SchemaPath, state.GlobalPath, state.StudyPaths);
    state.bLoaded = state.Bundle.Schema.bLoaded && state.Bundle.GlobalDocument.bLoaded;

    if (!state.SelectedStudyId.empty()) {
        state.SelectedStudyIndex = FindStudyIndexByIdV2(state, state.SelectedStudyId);
    }
    if (state.SelectedStudyIndex < 0 || state.SelectedStudyIndex >= static_cast<int>(state.Bundle.StudyDocuments.size())) {
        state.SelectedStudyIndex = state.Bundle.StudyDocuments.empty() ? 0 : 0;
        state.SelectedStudyId = state.Bundle.StudyDocuments.empty()
            ? Slab::Str{}
            : state.Bundle.StudyDocuments.front().Study.Id;
    }

    RebuildOntologyProjectionV2(state);
    state.Status = BuildOntologyStatusSummaryV2(state);
    state.bDirty = false;
}

auto FLabV2WindowManager::EnsureOntologyGraphWindow() -> void {
    using namespace Slab::Graphics::Plot2D::V2;

    if (OntologyGraphWindowV2 != nullptr && OntologyGraphArtistV2 != nullptr) return;

    auto window = Slab::New<FPlot2DWindowV2>(
        WindowTitleOntologyGraph,
        Slab::Graphics::RectR{-12.0, 54.0, -24.0, 8.0},
        Slab::Graphics::RectI{0, 1320, 0, 920});
    window->SetWindowId(OntologyGraphWindowId);
    window->SetAutoFitRanges(false);

    auto background = Slab::New<FBackgroundArtistV2>(Slab::Graphics::FColor::FromHex("#0A111B"));
    background->SetUseThemeColor(false);
    background->SetAffectGraphRanges(false);

    auto graphArtist = Slab::New<FOntologyGraphArtistV2>();
    graphArtist->SetOnSelectionChanged([this](const OntologyV2::FOntologyGraphSelectionV2 &selection) {
        OntologyWorkspaceState.Selection = selection;
    });

    window->AddArtist(background, -100);
    window->AddArtist(graphArtist, 0);

    OntologyGraphWindowV2 = window;
    OntologyGraphArtistV2 = graphArtist;
    PlotWindowsV2.push_back(window);
}

auto FLabV2WindowManager::SyncOntologyGraphWindow() -> void {
    using namespace Slab::Graphics::Plot2D::V2;

    EnsureOntologyWorkspaceData();
    auto &state = OntologyWorkspaceState;
    EnsureOntologyGraphWindow();
    if (OntologyGraphWindowV2 == nullptr || OntologyGraphArtistV2 == nullptr) return;

    const auto graphArtist = Slab::DynamicPointerCast<FOntologyGraphArtistV2>(OntologyGraphArtistV2);
    if (graphArtist == nullptr) return;

    graphArtist->SetProjection(state.Projection);
    graphArtist->SetSelection(state.Selection);
    graphArtist->SetShowEdgeLabels(state.Filters.bShowEdgeLabels);
    OntologyGraphWindowV2->SetTitle(WindowTitleOntologyGraph);

    if (state.bPendingGraphFit) {
        if (const auto it = PlotWindowHostsByWindowId.find(OntologyGraphWindowId);
            it != PlotWindowHostsByWindowId.end() && it->second != nullptr) {
            if (const auto host = Slab::DynamicPointerCast<FPlot2DWindowHostV2>(it->second);
                host != nullptr) {
                host->RequestFitToArtists();
                state.bPendingGraphFit = false;
            }
        }
    }
}

auto FLabV2WindowManager::FocusOntologyGraphWindow() -> void {
    EnsureOntologyWorkspaceData();
    EnsureOntologyGraphWindow();
    SetActiveWorkspace(EWorkspaceTab::Ontology);
    bShowWindowOntologyGraph = true;
    SyncOntologyGraphWindow();
    bPendingFocusOntologyGraphWindow = true;

    if (const auto it = PlotWindowHostsByWindowId.find(OntologyGraphWindowId);
        it != PlotWindowHostsByWindowId.end() && it->second != nullptr) {
        FocusWindow(it->second);
    }
}

auto FLabV2WindowManager::DrawOntologyLayerPanel() -> void {
    EnsureOntologyWorkspaceData();
    auto &state = OntologyWorkspaceState;

    ImGui::TextWrapped("Read-only graph projection over the ontology JSON contract under Resources/Ontologies.");
    ImGui::SeparatorText("Files");
    ImGui::TextDisabled("Directory: %s", state.ResourceDirectory.empty() ? "<missing>" : state.ResourceDirectory.string().c_str());
    ImGui::TextDisabled("Schema: %s", state.SchemaPath.empty() ? "<missing>" : state.SchemaPath.filename().string().c_str());
    ImGui::TextDisabled("Global: %s", state.GlobalPath.empty() ? "<missing>" : state.GlobalPath.filename().string().c_str());
    ImGui::TextDisabled("Studies: %zu", state.Bundle.StudyDocuments.size());

    const auto statusTint = state.Status.starts_with("[Ok]")
        ? ImVec4(0.30f, 0.82f, 0.42f, 1.0f)
        : ImVec4(0.88f, 0.34f, 0.30f, 1.0f);
    if (!state.Status.empty()) {
        ImGui::TextColored(statusTint, "%s", state.Status.c_str());
    }

    bool bNeedsRebuild = false;

    if (ImGui::Button("Reload Ontologies")) {
        state.bDirty = true;
        EnsureOntologyWorkspaceData();
        bNeedsRebuild = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Open Graph")) {
        FocusOntologyGraphWindow();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Filters")) {
        state.Filters = {};
        bNeedsRebuild = true;
    }

    ImGui::SeparatorText("Study");
    if (!state.Bundle.StudyDocuments.empty()) {
        if (state.SelectedStudyIndex < 0 || state.SelectedStudyIndex >= static_cast<int>(state.Bundle.StudyDocuments.size())) {
            state.SelectedStudyIndex = 0;
        }

        const auto &selectedStudy = state.Bundle.StudyDocuments[static_cast<std::size_t>(state.SelectedStudyIndex)];
        if (ImGui::BeginCombo("Selected Study", selectedStudy.Study.Title.c_str())) {
            for (std::size_t index = 0; index < state.Bundle.StudyDocuments.size(); ++index) {
                const auto &study = state.Bundle.StudyDocuments[index];
                const bool bSelected = static_cast<int>(index) == state.SelectedStudyIndex;
                if (ImGui::Selectable(study.Study.Title.c_str(), bSelected)) {
                    state.SelectedStudyIndex = static_cast<int>(index);
                    state.SelectedStudyId = study.Study.Id;
                    state.Selection = {};
                    bNeedsRebuild = true;
                }
                if (bSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::TextDisabled("%s", selectedStudy.SourceDocument.c_str());
        if (!selectedStudy.Study.Description.empty()) {
            ImGui::TextWrapped("%s", selectedStudy.Study.Description.c_str());
        }
    } else {
        ImGui::TextDisabled("No study documents were discovered.");
    }

    ImGui::SeparatorText("Filters");
    static constexpr std::array<const char *, 3> ScopeLabels{
        "Show All",
        "Global Only",
        "Selected Study Only"
    };
    auto scopeIndex = static_cast<int>(state.Filters.ScopeMode);
    if (ImGui::Combo("Scope", &scopeIndex, ScopeLabels.data(), static_cast<int>(ScopeLabels.size()))) {
        state.Filters.ScopeMode = static_cast<OntologyV2::EOntologyFilterScopeModeV2>(scopeIndex);
        bNeedsRebuild = true;
    }

    bNeedsRebuild |= ImGui::Checkbox("Hide ambient layer", &state.Filters.bHideAmbientLayer);
    bNeedsRebuild |= ImGui::Checkbox("Hide abstract nodes", &state.Filters.bHideAbstractNodes);
    bNeedsRebuild |= ImGui::Checkbox("Show only active/reachable region", &state.Filters.bFocusActiveReachableRegion);
    bNeedsRebuild |= ImGui::Checkbox("Show blocked requirements only", &state.Filters.bShowBlockedRequirementsOnly);
    bNeedsRebuild |= ImGui::Checkbox("Show realization/recipe/artifact path only", &state.Filters.bShowRealizationRecipeArtifactPathOnly);
    bNeedsRebuild |= ImGui::Checkbox("Show edge labels", &state.Filters.bShowEdgeLabels);

    if (bNeedsRebuild) {
        RebuildOntologyProjectionV2(state);
        SyncOntologyGraphWindow();
    }

    ImGui::SeparatorText("Projection");
    ImGui::Text("Visible nodes: %zu", state.Projection.Nodes.size());
    ImGui::SameLine();
    ImGui::TextDisabled("| visible edges: %zu", state.Projection.Edges.size());

    if (ImGui::CollapsingHeader("Diagnostics", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (state.Bundle.Diagnostics.Messages.empty()) {
            ImGui::TextDisabled("No diagnostics.");
        } else if (ImGui::BeginChild("OntologyDiagnostics", ImVec2(0.0f, 220.0f), true)) {
            for (const auto &diagnostic : state.Bundle.Diagnostics.Messages) {
                const auto tint = diagnostic.Severity == Slab::Core::Model::V2::EValidationSeverityV2::Error
                    ? ImVec4(0.90f, 0.32f, 0.30f, 1.0f)
                    : ImVec4(0.86f, 0.76f, 0.36f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, tint);
                ImGui::TextUnformatted((diagnostic.Code + " | " + diagnostic.EntityId).c_str());
                ImGui::PopStyleColor();
                ImGui::TextWrapped("%s", diagnostic.Message.c_str());
                if (!diagnostic.SourceDocument.empty()) {
                    ImGui::TextDisabled("%s", diagnostic.SourceDocument.c_str());
                }
                ImGui::Separator();
            }
            ImGui::EndChild();
        }
    }
}

auto FLabV2WindowManager::DrawOntologyDetailsPanel() -> void {
    EnsureOntologyWorkspaceData();
    auto &state = OntologyWorkspaceState;

    if (!state.Selection.IsValid()) {
        ImGui::TextDisabled("Select a node or edge in Ontology Graph.");
        return;
    }

    const auto selectNode = [&](const Slab::Str &nodeId) {
        state.Selection = {OntologyV2::EOntologyElementKindV2::Node, nodeId};
        SyncOntologyGraphWindow();
    };
    const auto selectEdge = [&](const Slab::Str &edgeId) {
        state.Selection = {OntologyV2::EOntologyElementKindV2::Edge, edgeId};
        SyncOntologyGraphWindow();
    };

    if (state.Selection.Kind == OntologyV2::EOntologyElementKindV2::Node) {
        const auto *node = state.Bundle.FindNode(state.Selection.ElementId);
        const auto *projectedNode = state.Projection.FindNode(state.Selection.ElementId);
        if (node == nullptr) {
            ImGui::TextDisabled("Selected node is no longer available.");
            return;
        }

        ImGui::TextUnformatted(node->Title.c_str());
        ImGui::TextDisabled("%s", node->Id.c_str());
        ImGui::Text("Ownership: %s", OntologyV2::ToString(node->OwnershipScope));
        ImGui::SameLine();
        ImGui::TextDisabled("| source: %s", node->SourceDocument.c_str());
        const auto friendlyCategory = OntologyV2::FriendlyNodeCategoryLabelV2(node->Kind, node->Layer);
        ImGui::Text("Category: %s", friendlyCategory.c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("| kind: %s", node->Kind.c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("| layer: %s", node->Layer.c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("| abstract: %s", node->bAbstract ? "yes" : "no");
        if (projectedNode != nullptr && projectedNode->ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None) {
            ImGui::Text("Status: %s", OntologyV2::ToString(projectedNode->ActivationStatus));
        }

        DrawJsonPayloadBlockV2("Properties", crude_json::value(node->Properties));

        ImGui::SeparatorText("Outgoing");
        if (const auto it = state.Bundle.OutgoingEdgesByNodeId.find(node->Id); it != state.Bundle.OutgoingEdgesByNodeId.end()) {
            for (const auto &edgeId : it->second) {
                const auto *edge = state.Bundle.FindEdge(edgeId);
                if (edge == nullptr) continue;
                const auto *target = state.Bundle.FindNode(edge->To);
                ImGui::PushID(edgeId.c_str());
                if (ImGui::SmallButton("Edge")) {
                    selectEdge(edgeId);
                }
                ImGui::SameLine();
                ImGui::Text("%s -> %s", edge->Type.c_str(), target != nullptr ? target->Title.c_str() : edge->To.c_str());
                if (target != nullptr) {
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Target")) {
                        selectNode(target->Id);
                    }
                }
                ImGui::PopID();
            }
        } else {
            ImGui::TextDisabled("No outgoing connections.");
        }

        ImGui::SeparatorText("Incoming");
        if (const auto it = state.Bundle.IncomingEdgesByNodeId.find(node->Id); it != state.Bundle.IncomingEdgesByNodeId.end()) {
            for (const auto &edgeId : it->second) {
                const auto *edge = state.Bundle.FindEdge(edgeId);
                if (edge == nullptr) continue;
                const auto *source = state.Bundle.FindNode(edge->From);
                ImGui::PushID((edgeId + "::incoming").c_str());
                if (ImGui::SmallButton("Edge")) {
                    selectEdge(edgeId);
                }
                ImGui::SameLine();
                ImGui::Text("%s <- %s", edge->Type.c_str(), source != nullptr ? source->Title.c_str() : edge->From.c_str());
                if (source != nullptr) {
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Source")) {
                        selectNode(source->Id);
                    }
                }
                ImGui::PopID();
            }
        } else {
            ImGui::TextDisabled("No incoming connections.");
        }
    } else if (state.Selection.Kind == OntologyV2::EOntologyElementKindV2::Edge) {
        const auto *edge = state.Bundle.FindEdge(state.Selection.ElementId);
        const auto *projectedEdge = state.Projection.FindEdge(state.Selection.ElementId);
        if (edge == nullptr) {
            ImGui::TextDisabled("Selected edge is no longer available.");
            return;
        }

        const auto *source = state.Bundle.FindNode(edge->From);
        const auto *target = state.Bundle.FindNode(edge->To);

        ImGui::TextUnformatted(edge->Type.c_str());
        ImGui::TextDisabled("%s", edge->Id.c_str());
        ImGui::Text("Ownership: %s", OntologyV2::ToString(edge->OwnershipScope));
        ImGui::SameLine();
        ImGui::TextDisabled("| source: %s", edge->SourceDocument.c_str());
        if (projectedEdge != nullptr && projectedEdge->ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None) {
            ImGui::Text("Status: %s", OntologyV2::ToString(projectedEdge->ActivationStatus));
        }

        ImGui::SeparatorText("Endpoints");
        if (source != nullptr) {
            if (ImGui::SmallButton("From")) {
                selectNode(source->Id);
            }
            ImGui::SameLine();
            ImGui::Text("%s", source->Title.c_str());
        } else {
            ImGui::TextDisabled("Missing source: %s", edge->From.c_str());
        }

        if (target != nullptr) {
            if (ImGui::SmallButton("To")) {
                selectNode(target->Id);
            }
            ImGui::SameLine();
            ImGui::Text("%s", target->Title.c_str());
        } else {
            ImGui::TextDisabled("Missing target: %s", edge->To.c_str());
        }

        DrawJsonPayloadBlockV2("Properties", crude_json::value(edge->Properties));
    }

    if (ImGui::CollapsingHeader("Diagnostics")) {
        bool bAny = false;
        for (const auto &diagnostic : state.Bundle.Diagnostics.Messages) {
            if (diagnostic.EntityId != state.Selection.ElementId) continue;
            bAny = true;
            const auto tint = diagnostic.Severity == Slab::Core::Model::V2::EValidationSeverityV2::Error
                ? ImVec4(0.90f, 0.32f, 0.30f, 1.0f)
                : ImVec4(0.86f, 0.76f, 0.36f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, tint);
            ImGui::TextUnformatted(diagnostic.Code.c_str());
            ImGui::PopStyleColor();
            ImGui::TextWrapped("%s", diagnostic.Message.c_str());
            ImGui::Separator();
        }
        if (!bAny) {
            ImGui::TextDisabled("No diagnostics for the current selection.");
        }
    }
}
