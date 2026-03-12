#include "LabV2WindowManager.h"

#include "Core/Model/V2/ModelSeedsV2.h"
#include "Graphics/Typesetting/ImGuiTypesetting.h"
#include "imgui.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>

namespace {

    namespace ModelV2 = Slab::Core::Model::V2;
    namespace Typesetting = Slab::Graphics::Typesetting;

    auto SetTextBufferFromString(const Slab::Str &value, char *buffer, const std::size_t size) -> void {
        if (buffer == nullptr || size == 0) return;
        std::snprintf(buffer, size, "%s", value.c_str());
    }

    auto AddTooltipForLastItem(const char *tooltip,
                               const ImGuiHoveredFlags flags = ImGuiHoveredFlags_DelayShort) -> void {
        if (tooltip == nullptr || tooltip[0] == '\0') return;
        if (!ImGui::IsItemHovered(flags)) return;
        if (!ImGui::BeginTooltip()) return;
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 36.0f);
        ImGui::TextUnformatted(tooltip);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    auto AddTooltipForLastItem(const Slab::Str &tooltip,
                               const ImGuiHoveredFlags flags = ImGuiHoveredFlags_DelayShort) -> void {
        if (tooltip.empty()) return;
        AddTooltipForLastItem(tooltip.c_str(), flags);
    }

    auto MakeMathRequest(const Slab::Str &latex, const float fontPixels) -> Typesetting::FTypesetRequest {
        Typesetting::FTypesetStyle style;
        style.FontPixelSize = fontPixels;
        return Typesetting::MakeMathRequest(latex, style);
    }

    struct FModelInspectorTheme {
        float BaseFontSize = 0.0f;
        Typesetting::FImGuiTypesetDrawOptions SectionTextOptions;
        Typesetting::FImGuiTypesetDrawOptions SecondaryTextOptions;
        Typesetting::FImGuiTypesetDrawOptions DetailMathOptions;
    };

    auto MakeSelectableRowOptions(const ImVec4 &accentColor,
                                  const bool bSelected) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        Typesetting::FImGuiSelectableTypesetRowOptions rowOptions;
        rowOptions.bSelected = bSelected;
        rowOptions.PrimaryTint = ImGui::GetColorU32(ImGuiCol_Text);
        rowOptions.SecondaryTint = ImGui::GetColorU32(ImGuiCol_TextDisabled);
        rowOptions.HoveredBackgroundTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.12f));
        rowOptions.SelectedBackgroundTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.20f));
        rowOptions.HoveredBorderTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.70f));
        rowOptions.SelectedBorderTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.95f));
        rowOptions.HoveredAccentTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.78f));
        rowOptions.SelectedAccentTint = ImGui::GetColorU32(accentColor);
        return rowOptions;
    }

    auto MakeDefinitionRowOptions(const bool bSelected) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        return MakeSelectableRowOptions(ImVec4(0.29f, 0.60f, 0.45f, 1.0f), bSelected);
    }

    auto MakeRelationRowOptions(const bool bSelected) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        return MakeSelectableRowOptions(ImVec4(0.31f, 0.49f, 0.74f, 1.0f), bSelected);
    }

    auto FindDefinitionReport(const ModelV2::FModelSemanticReportV2 &report,
                              const Slab::Str &definitionId) -> const ModelV2::FDefinitionSemanticReportV2 * {
        const auto it = std::find_if(report.Definitions.begin(), report.Definitions.end(), [&](const auto &entry) {
            return entry.DefinitionId == definitionId;
        });
        if (it == report.Definitions.end()) return nullptr;
        return &(*it);
    }

    auto FindRelationReport(const ModelV2::FModelSemanticReportV2 &report,
                            const Slab::Str &relationId) -> const ModelV2::FRelationSemanticReportV2 * {
        const auto it = std::find_if(report.Relations.begin(), report.Relations.end(), [&](const auto &entry) {
            return entry.RelationId == relationId;
        });
        if (it == report.Relations.end()) return nullptr;
        return &(*it);
    }

    auto MakeEditorBufferKey(const ModelV2::FModelV2 &model,
                             const ModelV2::EModelObjectKindV2 kind,
                             const Slab::Str &objectId) -> Slab::Str {
        return model.ModelId + "::" + ModelV2::ToString(kind) + "::" + objectId;
    }

    auto EnsureEditorBuffer(std::map<Slab::Str, ModelV2::FModelEditorBufferV2> &buffersByKey,
                            const ModelV2::FModelV2 &model,
                            const ModelV2::EModelObjectKindV2 kind,
                            const Slab::Str &objectId) -> ModelV2::FModelEditorBufferV2 * {
        if (objectId.empty()) return nullptr;

        const auto key = MakeEditorBufferKey(model, kind, objectId);
        const auto canonicalNotation = kind == ModelV2::EModelObjectKindV2::Definition
            ? (ModelV2::FindDefinitionByIdV2(model, objectId) != nullptr
                   ? ModelV2::MakeCanonicalDefinitionNotationV2(*ModelV2::FindDefinitionByIdV2(model, objectId), &model)
                   : Slab::Str{})
            : (ModelV2::FindRelationByIdV2(model, objectId) != nullptr
                   ? ModelV2::MakeCanonicalRelationNotationV2(*ModelV2::FindRelationByIdV2(model, objectId), &model)
                   : Slab::Str{});
        if (canonicalNotation.empty()) return nullptr;

        const auto it = buffersByKey.find(key);
        if (it == buffersByKey.end()) {
            const auto created = kind == ModelV2::EModelObjectKindV2::Definition
                ? ModelV2::MakeEditorBufferForDefinitionV2(model, objectId)
                : ModelV2::MakeEditorBufferForRelationV2(model, objectId);
            if (!created.has_value()) return nullptr;
            const auto insertResult = buffersByKey.insert({key, *created});
            return &insertResult.first->second;
        }

        auto &buffer = it->second;
        if (buffer.CanonicalNotation != canonicalNotation && !buffer.bDraftDirty) {
            ModelV2::RevertEditorBufferV2(model, buffer);
        }
        return &buffer;
    }

    auto DrawDiagnosticList(const Slab::Vector<ModelV2::FSemanticDiagnosticV2> &diagnostics) -> void {
        if (diagnostics.empty()) return;
        ImGui::SeparatorText("Diagnostics");
        for (const auto &diagnostic : diagnostics) {
            const auto tint = diagnostic.Severity == ModelV2::EValidationSeverityV2::Error
                ? ImVec4(0.82f, 0.26f, 0.26f, 1.0f)
                : ImVec4(0.87f, 0.67f, 0.22f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, tint);
            ImGui::BulletText("[%s] %s: %s", diagnostic.Code.c_str(), diagnostic.Context.c_str(), diagnostic.Message.c_str());
            ImGui::PopStyleColor();
            AddTooltipForLastItem(
                diagnostic.Severity == ModelV2::EValidationSeverityV2::Error
                    ? "This is a blocking semantic diagnostic. Apply stays disabled until errors are resolved."
                    : "This is a semantic warning. It highlights inferred, deferred, or possibly inconsistent meaning without blocking Apply.");
        }
    }

    auto DrawReferencedSymbols(const Slab::Vector<ModelV2::FReferencedSymbolSemanticV2> &symbols) -> void {
        if (symbols.empty()) return;
        ImGui::SeparatorText("Referenced Symbols");
        for (const auto &symbol : symbols) {
            ImGui::PushID(symbol.SymbolText.c_str());
            ImGui::BulletText(
                "%s | %s",
                symbol.SymbolText.c_str(),
                symbol.bResolved ? "resolved" : "unresolved");
            AddTooltipForLastItem(
                symbol.bResolved
                    ? "This referenced symbol is already bound to a canonical Definition in the model."
                    : "This referenced symbol parsed successfully but is not yet declared canonically. It can remain implicit or be accepted as an explicit Definition.");
            if (!symbol.ReferenceId.empty()) {
                ImGui::TextDisabled("Definition: %s", symbol.ReferenceId.c_str());
                AddTooltipForLastItem("Canonical Definition id currently bound to this symbol reference.");
            }
            if (!symbol.UsageLabels.empty()) {
                Slab::Str usageSummary;
                for (std::size_t i = 0; i < symbol.UsageLabels.size(); ++i) {
                    if (i != 0) usageSummary += ", ";
                    usageSummary += symbol.UsageLabels[i];
                }
                ImGui::TextDisabled("Uses: %s", usageSummary.c_str());
                AddTooltipForLastItem("Usage labels summarize how the parser and semantic pass encountered this symbol inside the selected notation.");
            }
            if (symbol.DeclaredKind.has_value()) {
                ImGui::TextDisabled("Declared: %s", ModelV2::ToString(*symbol.DeclaredKind));
                AddTooltipForLastItem("Current canonical role declared for this symbol.");
            }
            if (symbol.InferredKind.has_value()) {
                ImGui::TextDisabled("Inferred: %s", ModelV2::ToString(*symbol.InferredKind));
                AddTooltipForLastItem("Role inferred from how this symbol is used in the current expression and model context.");
            }
            if (!symbol.InferredArgumentDefinitionIds.empty()) {
                Slab::Str deps;
                for (std::size_t i = 0; i < symbol.InferredArgumentDefinitionIds.size(); ++i) {
                    if (i != 0) deps += ", ";
                    deps += symbol.InferredArgumentDefinitionIds[i];
                }
                ImGui::TextDisabled("Depends on: %s", deps.c_str());
                AddTooltipForLastItem("These dependencies were inferred from usage, for example from derivative structure or function arguments.");
            }
            if (symbol.bDeclaredInferredAgreement) {
                ImGui::TextDisabled("Declared vs inferred: agreement");
                AddTooltipForLastItem("The inferred role agrees with the currently declared canonical role.");
            } else if (symbol.bDeclaredInferredMismatch) {
                ImGui::TextColored(ImVec4(0.87f, 0.67f, 0.22f, 1.0f), "Declared vs inferred: mismatch");
                AddTooltipForLastItem("The inferred role disagrees with the currently declared canonical role. This is a trust signal rather than an automatic rewrite.");
            }
            ImGui::PopID();
        }
    }

    auto DrawAssumptionSummary(const Slab::Vector<ModelV2::FSemanticAssumptionV2> &assumptions) -> void {
        if (assumptions.empty()) return;
        ImGui::SeparatorText("Assumptions");
        for (const auto &assumption : assumptions) {
            ImGui::PushID(assumption.AssumptionId.c_str());
            ImGui::BulletText(
                "%s | %s | %s",
                assumption.Category.c_str(),
                assumption.TargetSymbol.empty() ? assumption.TargetId.c_str() : assumption.TargetSymbol.c_str(),
                ModelV2::ToString(assumption.Status));
            AddTooltipForLastItem(
                "This semantic proposal was inferred from notation and context. It is not canonical model state unless explicitly accepted.");
            if (!assumption.SourceId.empty()) {
                ImGui::TextDisabled("Source: %s", assumption.SourceId.c_str());
                AddTooltipForLastItem("The relation or object whose usage triggered this assumption.");
            }
            if (!assumption.Detail.empty()) {
                ImGui::TextWrapped("%s", assumption.Detail.c_str());
                AddTooltipForLastItem("Human-readable explanation of why this assumption was inferred.");
            }
            ImGui::PopID();
        }
    }

    auto DrawDefinitionInspector(Typesetting::FTypesettingService &typesettingService,
                                 const ModelV2::FModelV2 &model,
                                 const ModelV2::FDefinitionV2 &definition,
                                 const ModelV2::FDefinitionSemanticReportV2 *report,
                                 const ModelV2::FDefinitionDraftPreviewV2 *draftPreview,
                                 const FModelInspectorTheme &theme) -> void {
        const auto title = definition.DisplayName.empty() ? definition.DefinitionId : definition.DisplayName;
        Typesetting::ImGuiTypesetText(typesettingService, title, theme.SectionTextOptions);
        ImGui::TextDisabled("%s | %s", definition.DefinitionId.c_str(), ModelV2::ToString(definition.Kind));
        AddTooltipForLastItem("Stable Definition id and its currently declared canonical role.");
        Typesetting::ImGuiTypesetMath(
            typesettingService,
            ModelV2::RenderDialectDefinitionV2(definition, &model),
            theme.DetailMathOptions);

        ImGui::TextDisabled("Canonical stored notation");
        AddTooltipForLastItem("This is the canonical notation currently stored on the selected Definition. Draft edits do not change it until Apply.");
        Typesetting::ImGuiTypesetMath(
            typesettingService,
            ModelV2::MakeCanonicalDefinitionNotationV2(definition, &model),
            theme.DetailMathOptions);

        if (draftPreview != nullptr && draftPreview->bParseOk) {
            ImGui::SeparatorText("Draft Preview");
            Typesetting::ImGuiTypesetMath(
                typesettingService,
                draftPreview->DraftNotation,
                theme.DetailMathOptions);
            Typesetting::ImGuiTypesetLabelValue(
                typesettingService,
                "Normalized",
                MakeMathRequest(draftPreview->NormalizedProjection, theme.BaseFontSize),
                theme.SecondaryTextOptions,
                theme.DetailMathOptions);
        }

        if (!definition.Description.empty()) {
            ImGui::TextWrapped("%s", definition.Description.c_str());
        }
        if (definition.DeclaredType.has_value()) {
            Typesetting::ImGuiTypesetLabelValue(
                typesettingService,
                "Declared Type",
                MakeMathRequest(ModelV2::RenderDialectTypeExprV2(*definition.DeclaredType, &model), theme.BaseFontSize),
                theme.SecondaryTextOptions,
                theme.DetailMathOptions);
        }

        ImGui::SeparatorText("Semantics");
        ImGui::BulletText("Declared role: %s", ModelV2::ToString(definition.Kind));
        if (report != nullptr && report->InferredKind.has_value()) {
            ImGui::BulletText("Inferred role: %s", ModelV2::ToString(*report->InferredKind));
            if (report->bRoleMatchesDeclared) {
                ImGui::TextDisabled("Declared vs inferred: agreement");
            } else if (report->bRoleMismatchesDeclared) {
                ImGui::TextColored(ImVec4(0.87f, 0.67f, 0.22f, 1.0f), "Declared vs inferred: mismatch");
            }
        }
        if (report != nullptr && !report->NormalizedInterpretation.empty()) {
            ImGui::TextWrapped("%s", report->NormalizedInterpretation.c_str());
        }

        Slab::Vector<Slab::Str> dependencies = definition.ArgumentDefinitionIds;
        for (const auto &dependencyId : definition.DependencyDefinitionIds) {
            if (dependencies.end() == std::find(dependencies.begin(), dependencies.end(), dependencyId)) {
                dependencies.push_back(dependencyId);
            }
        }
        if (report != nullptr) {
            for (const auto &dependencyId : report->InferredArgumentDefinitionIds) {
                if (dependencies.end() == std::find(dependencies.begin(), dependencies.end(), dependencyId)) {
                    dependencies.push_back(dependencyId);
                }
            }
        }
        if (!dependencies.empty()) {
            ImGui::SeparatorText("Dependencies");
            for (const auto &dependencyId : dependencies) {
                ImGui::BulletText("%s", dependencyId.c_str());
            }
        }
        if (report != nullptr && !report->UsedByRelationIds.empty()) {
            ImGui::SeparatorText("Used By");
            for (const auto &relationId : report->UsedByRelationIds) {
                ImGui::BulletText("%s", relationId.c_str());
            }
        }

        if (report != nullptr) DrawDiagnosticList(report->Diagnostics);
        if (draftPreview != nullptr && !draftPreview->Diagnostics.empty()) DrawDiagnosticList(draftPreview->Diagnostics);
        if (report != nullptr) DrawAssumptionSummary(report->Assumptions);
    }

    auto DrawRelationInspector(Typesetting::FTypesettingService &typesettingService,
                               const ModelV2::FModelV2 &model,
                               const ModelV2::FRelationV2 &relation,
                               const ModelV2::FRelationSemanticReportV2 *report,
                               const ModelV2::FRelationDraftPreviewV2 *draftPreview,
                               const FModelInspectorTheme &theme) -> void {
        const auto title = relation.Name.empty() ? relation.RelationId : relation.Name;
        Typesetting::ImGuiTypesetText(typesettingService, title, theme.SectionTextOptions);
        ImGui::TextDisabled("%s | %s", relation.RelationId.c_str(), ModelV2::ToString(relation.Kind));
        AddTooltipForLastItem("Stable Relation id and its currently declared canonical relation kind.");
        Typesetting::ImGuiTypesetMath(
            typesettingService,
            ModelV2::RenderDialectRelationV2(relation, &model),
            theme.DetailMathOptions);

        ImGui::TextDisabled("Canonical stored notation");
        AddTooltipForLastItem("This is the canonical notation currently stored on the selected Relation. Draft edits remain provisional until Apply.");
        Typesetting::ImGuiTypesetMath(
            typesettingService,
            ModelV2::MakeCanonicalRelationNotationV2(relation, &model),
            theme.DetailMathOptions);

        if (draftPreview != nullptr && draftPreview->bParseOk) {
            ImGui::SeparatorText("Draft Preview");
            Typesetting::ImGuiTypesetMath(
                typesettingService,
                draftPreview->DraftNotation,
                theme.DetailMathOptions);
            Typesetting::ImGuiTypesetLabelValue(
                typesettingService,
                "Normalized",
                MakeMathRequest(draftPreview->NormalizedProjection, theme.BaseFontSize),
                theme.SecondaryTextOptions,
                theme.DetailMathOptions);
        }

        if (!relation.Description.empty()) {
            ImGui::TextWrapped("%s", relation.Description.c_str());
        }

        ImGui::SeparatorText("Semantics");
        if (report != nullptr) {
            ImGui::BulletText("Inferred class: %s", ModelV2::ToString(report->InferredClass));
            if (!report->NormalizedInterpretation.empty()) {
                ImGui::TextWrapped("%s", report->NormalizedInterpretation.c_str());
            }
            DrawReferencedSymbols(report->ReferencedSymbols);
            DrawDiagnosticList(report->Diagnostics);
            DrawAssumptionSummary(report->Assumptions);
        }
        if (draftPreview != nullptr && draftPreview->bParseOk) {
            DrawReferencedSymbols(draftPreview->ReferencedSymbols);
            DrawDiagnosticList(draftPreview->Diagnostics);
            DrawAssumptionSummary(draftPreview->Assumptions);
        }
    }

} // namespace

auto FLabV2WindowManager::DrawModelInspectorPanel() -> void {
    using namespace Slab::Core::Model::V2;

    if (ModelDemoCatalog.empty()) {
        ModelDemoCatalog = BuildDemoModelsV2();
    }
    if (ModelDemoCatalog.empty()) {
        ImGui::TextDisabled("No demo models available.");
        return;
    }

    if (SelectedModelIndex < 0 || SelectedModelIndex >= static_cast<int>(ModelDemoCatalog.size())) {
        SelectedModelIndex = 0;
    }

    bool bModelChanged = false;
    if (ImGui::BeginCombo("Seed Model", ModelDemoCatalog[SelectedModelIndex].Name.c_str())) {
        for (int i = 0; i < static_cast<int>(ModelDemoCatalog.size()); ++i) {
            const bool bSelected = i == SelectedModelIndex;
            if (ImGui::Selectable(ModelDemoCatalog[i].Name.c_str(), bSelected)) {
                SelectedModelIndex = i;
                bModelChanged = true;
            }
            if (bSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    AddTooltipForLastItem("Switch between seeded reference models. Changing the seed resets pinned selection and editor draft state for this Model tab session.");

    auto &model = ModelDemoCatalog[SelectedModelIndex];
    if (bModelChanged) {
        SelectedModelDefinitionId.clear();
        SelectedModelRelationId.clear();
        SelectedModelAssumptionId.clear();
        bSelectedModelDetailIsRelation = false;
        ModelEditorBuffersByKey.clear();
        ModelEditorStatus.clear();
        bModelHasLastChangeRecord = false;
    }

    if (SelectedModelDefinitionId.empty() && !model.Definitions.empty()) {
        SelectedModelDefinitionId = model.Definitions.front().DefinitionId;
    }
    if (SelectedModelRelationId.empty() && !model.Relations.empty()) {
        SelectedModelRelationId = model.Relations.front().RelationId;
    }
    if (!SelectedModelDefinitionId.empty() && FindDefinitionByIdV2(model, SelectedModelDefinitionId) == nullptr && !model.Definitions.empty()) {
        SelectedModelDefinitionId = model.Definitions.front().DefinitionId;
    }
    if (!SelectedModelRelationId.empty() && FindRelationByIdV2(model, SelectedModelRelationId) == nullptr && !model.Relations.empty()) {
        SelectedModelRelationId = model.Relations.front().RelationId;
    }

    const auto validation = ValidateModelV2(model);
    const auto semanticReport = BuildModelSemanticReportV2(model);

    auto &typesettingService = *UiTypesettingService;
    const auto baseFontSize = ImGui::GetFontSize();
    FModelInspectorTheme inspectorTheme;
    inspectorTheme.BaseFontSize = baseFontSize;
    inspectorTheme.SectionTextOptions.Style.FontPixelSize = baseFontSize * 1.05f;
    inspectorTheme.SectionTextOptions.Tint = ImGui::GetColorU32(ImGuiCol_Text);
    inspectorTheme.SecondaryTextOptions.Style.FontPixelSize = baseFontSize * 0.92f;
    inspectorTheme.SecondaryTextOptions.Tint = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    inspectorTheme.DetailMathOptions.Style.FontPixelSize = baseFontSize * 1.12f;
    inspectorTheme.DetailMathOptions.Tint = ImGui::GetColorU32(ImGuiCol_Text);

    ImGui::SeparatorText((model.Name + " (" + model.ModelId + ")").c_str());
    ImGui::TextWrapped("%s", model.Description.c_str());
    ImGui::TextDisabled(
        "Definitions: %zu | Relations: %zu | Validation errors: %zu | Assumptions: %zu",
        model.Definitions.size(),
        model.Relations.size(),
        validation.ErrorCount(),
        semanticReport.Assumptions.size());
    AddTooltipForLastItem("Counts and validation summary for the current canonical model. Assumptions are inferred proposals, not automatically promoted definitions.");

    if (bModelHasLastChangeRecord) {
        ImGui::TextDisabled(
            "Last change: %s %s | %s",
            ToString(ModelLastChangeRecord.Origin),
            ToString(ModelLastChangeRecord.ObjectKind),
            ModelLastChangeRecord.ObjectId.c_str());
        AddTooltipForLastItem("Most recent canonical change recorded by the Model tab. This is structured to stay compatible with future provenance/version tracking.");
    }
    if (!ModelEditorStatus.empty()) {
        ImGui::TextWrapped("%s", ModelEditorStatus.c_str());
        AddTooltipForLastItem("Latest status message from parse, apply, revert, or assumption actions.");
    }

    if (!validation.IsOk()) {
        DrawDiagnosticList(semanticReport.Diagnostics);
    }

    const FDefinitionV2 *hoveredDefinition = nullptr;
    const FRelationV2 *hoveredRelation = nullptr;
    const float layoutWidth = ImGui::GetContentRegionAvail().x;
    const float layoutHeight = std::max(ImGui::GetContentRegionAvail().y, 0.0f);
    const float listColumnWidth = std::clamp(layoutWidth * 0.32f, 260.0f, 380.0f);
    const float layoutSplitterThickness = 6.0f;
    const float minScratchpadHeight = std::clamp(layoutHeight * 0.24f, 160.0f, 260.0f);
    const float minTopPaneHeight = std::clamp(layoutHeight * 0.35f, 96.0f, 260.0f);
    const float maxScratchpadHeight =
        std::max(minScratchpadHeight, layoutHeight - minTopPaneHeight - layoutSplitterThickness);
    float scratchpadHeight =
        std::clamp(ModelScratchpadHeight, minScratchpadHeight, maxScratchpadHeight);
    const float topPaneHeight =
        std::max(0.0f, layoutHeight - scratchpadHeight - layoutSplitterThickness);

    ImGui::TextDisabled("Hover previews the inspector. The transactional editor always follows the pinned selection.");
    AddTooltipForLastItem("Hovering a row temporarily changes only the inspector. The editor remains bound to the clicked selection so drafts do not jump around.");

    if (topPaneHeight > 0.0f) {
        ImGui::BeginChild("ModelTopPane", ImVec2(0.0f, topPaneHeight), false, ImGuiWindowFlags_NoScrollbar);

        if (ImGui::BeginTable(
                "ModelCatalogLayout",
                2,
                ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Catalog", ImGuiTableColumnFlags_WidthFixed, listColumnWidth);
            ImGui::TableSetupColumn("Inspector", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextColumn();

            const float splitterThickness = 6.0f;
            const float catalogRailHeight = std::max(ImGui::GetContentRegionAvail().y, 0.0f);
            const float minCatalogPaneHeight = std::max(
                48.0f,
                std::min(
                    std::max(baseFontSize * 4.0f, 96.0f),
                    std::max(48.0f, (catalogRailHeight - splitterThickness) * 0.4f)));
            const float maxDefinitionsHeight =
                std::max(minCatalogPaneHeight, catalogRailHeight - minCatalogPaneHeight - splitterThickness);
            float definitionsPanelHeight =
                std::clamp(ModelCatalogDefinitionsHeight, minCatalogPaneHeight, maxDefinitionsHeight);

            ImGui::BeginChild("ModelCatalogRail", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_NoScrollbar);
            ImGui::BeginChild("ModelDefinitionsList", ImVec2(0.0f, definitionsPanelHeight), true);
            ImGui::SeparatorText("Definitions");
            for (const auto &definition : model.Definitions) {
                const bool bSelected =
                    !bSelectedModelDetailIsRelation &&
                    definition.DefinitionId == SelectedModelDefinitionId;
                ImGui::PushID(definition.DefinitionId.c_str());
                const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                    typesettingService,
                    "##ModelDefinitionRow",
                    MakeMathRequest(RenderDialectDefinitionV2(definition, &model), baseFontSize * 1.02f),
                    MakeDefinitionRowOptions(bSelected));
                AddTooltipForLastItem(
                    "Click to pin this Definition as the editor target. Hover only previews it in the inspector.");
                ImGui::PopID();

                if (rowResult.bHovered) hoveredDefinition = &definition;
                if (rowResult.bPressed) {
                    SelectedModelDefinitionId = definition.DefinitionId;
                    bSelectedModelDetailIsRelation = false;
                }
            }
            ImGui::EndChild();

            ImGui::InvisibleButton("##ModelCatalogSplitter", ImVec2(-FLT_MIN, splitterThickness));
            const bool bSplitterHovered = ImGui::IsItemHovered();
            const bool bSplitterActive = ImGui::IsItemActive();
            if (bSplitterHovered || bSplitterActive) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            if (bSplitterActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
                definitionsPanelHeight = std::clamp(
                    definitionsPanelHeight + ImGui::GetIO().MouseDelta.y,
                    minCatalogPaneHeight,
                    maxDefinitionsHeight);
                ModelCatalogDefinitionsHeight = definitionsPanelHeight;
            } else {
                ModelCatalogDefinitionsHeight = definitionsPanelHeight;
            }

            const auto splitterRectMin = ImGui::GetItemRectMin();
            const auto splitterRectMax = ImGui::GetItemRectMax();
            const float splitterCenterY = 0.5f * (splitterRectMin.y + splitterRectMax.y);
            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(splitterRectMin.x + 2.0f, splitterCenterY),
                ImVec2(splitterRectMax.x - 2.0f, splitterCenterY),
                bSplitterActive ? IM_COL32(246, 202, 116, 210) : IM_COL32(96, 108, 124, 192),
                bSplitterActive ? 2.0f : 1.0f);

            ImGui::BeginChild("ModelRelationsList", ImVec2(0.0f, 0.0f), true);
            ImGui::SeparatorText("Relations");
            for (const auto &relation : model.Relations) {
                const bool bSelected =
                    bSelectedModelDetailIsRelation &&
                    relation.RelationId == SelectedModelRelationId;
                ImGui::PushID(relation.RelationId.c_str());
                const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                    typesettingService,
                    "##ModelRelationRow",
                    MakeMathRequest(RenderDialectRelationV2(relation, &model), baseFontSize * 1.04f),
                    MakeRelationRowOptions(bSelected));
                AddTooltipForLastItem(
                    "Click to pin this Relation as the editor target. Hover only previews it in the inspector.");
                ImGui::PopID();

                if (rowResult.bHovered) hoveredRelation = &relation;
                if (rowResult.bPressed) {
                    SelectedModelRelationId = relation.RelationId;
                    bSelectedModelDetailIsRelation = true;
                }
            }
            ImGui::EndChild();
            ImGui::EndChild();

            const auto *selectedDefinition = FindDefinitionByIdV2(model, SelectedModelDefinitionId);
            const auto *selectedRelation = FindRelationByIdV2(model, SelectedModelRelationId);
            const bool bFocusFromHover = hoveredDefinition != nullptr || hoveredRelation != nullptr;
            const bool bFocusIsRelation = hoveredRelation != nullptr ||
                (hoveredDefinition == nullptr && bSelectedModelDetailIsRelation);
            const auto *focusedDefinition = hoveredDefinition != nullptr ? hoveredDefinition : selectedDefinition;
            const auto *focusedRelation = hoveredRelation != nullptr ? hoveredRelation : selectedRelation;

            const auto *pinnedDefinitionReport =
                selectedDefinition != nullptr ? FindDefinitionReport(semanticReport, selectedDefinition->DefinitionId) : nullptr;
            const auto *pinnedRelationReport =
                selectedRelation != nullptr ? FindRelationReport(semanticReport, selectedRelation->RelationId) : nullptr;

            ModelV2::FModelEditorBufferV2 *pinnedEditorBuffer = nullptr;
            if (bSelectedModelDetailIsRelation && selectedRelation != nullptr) {
                pinnedEditorBuffer = EnsureEditorBuffer(
                    ModelEditorBuffersByKey,
                    model,
                    EModelObjectKindV2::Relation,
                    selectedRelation->RelationId);
            } else if (selectedDefinition != nullptr) {
                pinnedEditorBuffer = EnsureEditorBuffer(
                    ModelEditorBuffersByKey,
                    model,
                    EModelObjectKindV2::Definition,
                    selectedDefinition->DefinitionId);
            }

            const auto *activeDefinitionDraft =
                selectedDefinition != nullptr &&
                    pinnedEditorBuffer != nullptr &&
                    pinnedEditorBuffer->TargetKind == EModelObjectKindV2::Definition &&
                    pinnedEditorBuffer->TargetId == selectedDefinition->DefinitionId &&
                    pinnedEditorBuffer->DefinitionPreview.has_value()
                ? &(*pinnedEditorBuffer->DefinitionPreview)
                : nullptr;
            const auto *activeRelationDraft =
                selectedRelation != nullptr &&
                    pinnedEditorBuffer != nullptr &&
                    pinnedEditorBuffer->TargetKind == EModelObjectKindV2::Relation &&
                    pinnedEditorBuffer->TargetId == selectedRelation->RelationId &&
                    pinnedEditorBuffer->RelationPreview.has_value()
                ? &(*pinnedEditorBuffer->RelationPreview)
                : nullptr;

            ImGui::TableNextColumn();
            ImGui::BeginChild("ModelFocusedDetailsPane", ImVec2(0.0f, 0.0f), true);
            ImGui::SeparatorText("Inspector");
            if (focusedRelation != nullptr && bFocusIsRelation) {
                ImGui::TextDisabled("Focused relation | Source: %s", bFocusFromHover ? "hover preview" : "pinned selection");
                AddTooltipForLastItem(
                    bFocusFromHover
                        ? "The inspector is currently showing a temporary hover preview. The editor target remains the pinned selection."
                        : "The inspector is showing the pinned selection, which is also the editor target.");
                DrawRelationInspector(
                    typesettingService,
                    model,
                    *focusedRelation,
                    FindRelationReport(semanticReport, focusedRelation->RelationId),
                    focusedRelation == selectedRelation ? activeRelationDraft : nullptr,
                    inspectorTheme);
            } else if (focusedDefinition != nullptr) {
                ImGui::TextDisabled("Focused definition | Source: %s", bFocusFromHover ? "hover preview" : "pinned selection");
                AddTooltipForLastItem(
                    bFocusFromHover
                        ? "The inspector is currently showing a temporary hover preview. The editor target remains the pinned selection."
                        : "The inspector is showing the pinned selection, which is also the editor target.");
                DrawDefinitionInspector(
                    typesettingService,
                    model,
                    *focusedDefinition,
                    FindDefinitionReport(semanticReport, focusedDefinition->DefinitionId),
                    focusedDefinition == selectedDefinition ? activeDefinitionDraft : nullptr,
                    inspectorTheme);
            } else {
                ImGui::TextDisabled("Select a definition or relation.");
            }
            ImGui::EndChild();
            ImGui::EndTable();
        }

        ImGui::EndChild();
    }

    ImGui::InvisibleButton("##ModelScratchpadSplitter", ImVec2(-FLT_MIN, layoutSplitterThickness));
    const bool bScratchpadSplitterHovered = ImGui::IsItemHovered();
    const bool bScratchpadSplitterActive = ImGui::IsItemActive();
    if (bScratchpadSplitterHovered || bScratchpadSplitterActive) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    if (bScratchpadSplitterActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
        scratchpadHeight = std::clamp(
            scratchpadHeight - ImGui::GetIO().MouseDelta.y,
            minScratchpadHeight,
            maxScratchpadHeight);
        ModelScratchpadHeight = scratchpadHeight;
    } else {
        ModelScratchpadHeight = scratchpadHeight;
    }

    const auto scratchpadSplitterRectMin = ImGui::GetItemRectMin();
    const auto scratchpadSplitterRectMax = ImGui::GetItemRectMax();
    const float scratchpadSplitterCenterY =
        0.5f * (scratchpadSplitterRectMin.y + scratchpadSplitterRectMax.y);
    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(scratchpadSplitterRectMin.x + 2.0f, scratchpadSplitterCenterY),
        ImVec2(scratchpadSplitterRectMax.x - 2.0f, scratchpadSplitterCenterY),
        bScratchpadSplitterActive ? IM_COL32(246, 202, 116, 210) : IM_COL32(96, 108, 124, 192),
        bScratchpadSplitterActive ? 2.0f : 1.0f);

    ImGui::BeginChild("ModelBottomPane", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_NoScrollbar);
    if (ImGui::BeginTable(
            "ModelBottomLayout",
            2,
            ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Editor", ImGuiTableColumnFlags_WidthStretch, 1.2f);
        ImGui::TableSetupColumn("Assumptions", ImGuiTableColumnFlags_WidthStretch, 0.8f);

        ImGui::TableNextColumn();
        ImGui::BeginChild("ModelEditorPane", ImVec2(0.0f, 0.0f), true);
        ImGui::SeparatorText("Notation Editor");

        const auto editorTargetKind = bSelectedModelDetailIsRelation ? EModelObjectKindV2::Relation : EModelObjectKindV2::Definition;
        const auto editorTargetId = bSelectedModelDetailIsRelation ? SelectedModelRelationId : SelectedModelDefinitionId;
        auto *editorBuffer = EnsureEditorBuffer(ModelEditorBuffersByKey, model, editorTargetKind, editorTargetId);
        if (editorBuffer == nullptr) {
            ImGui::TextDisabled("Select a definition or relation to edit.");
        } else {
            ImGui::TextDisabled(
                "Target: %s | Kind: %s | Editor source: %s",
                editorBuffer->TargetId.c_str(),
                ToString(editorBuffer->TargetKind),
                editorBuffer->bDraftDirty ? "draft" : "canonical stored notation");
            AddTooltipForLastItem("The editor is bound to this pinned object. 'draft' means the text differs from canonical stored notation.");
            ImGui::TextDisabled("Canonical");
            AddTooltipForLastItem("Current canonical notation for the selected object. It remains unchanged until Apply succeeds.");
            Typesetting::ImGuiTypesetMath(
                typesettingService,
                editorBuffer->CanonicalNotation,
                inspectorTheme.DetailMathOptions);

            char buffer[4096];
            SetTextBufferFromString(editorBuffer->DraftNotation, buffer, sizeof(buffer));
            if (ImGui::InputTextMultiline(
                    "##ModelEditorDraftNotation",
                    buffer,
                    sizeof(buffer),
                    ImVec2(-FLT_MIN, 120.0f),
                ImGuiInputTextFlags_AllowTabInput)) {
                SetEditorBufferDraftV2(*editorBuffer, buffer);
            }
            AddTooltipForLastItem("Editable draft notation. You can author math-like text here, preview the parsed semantics, and only then commit explicitly.");

            if (ImGui::Button("Parse / Preview")) {
                if (ParseEditorBufferPreviewV2(model, *editorBuffer)) {
                    ModelEditorStatus = "[Ok] Parsed draft for " + editorBuffer->TargetId + ".";
                } else {
                    ModelEditorStatus = "[Error] Could not parse draft for " + editorBuffer->TargetId + ".";
                }
            }
            AddTooltipForLastItem("Parse the draft without mutating canonical model state. The preview shows normalized notation, referenced symbols, inferred meaning, and diagnostics.");
            ImGui::SameLine();
            const bool bCanApply = editorBuffer->TargetKind == EModelObjectKindV2::Definition
                ? (editorBuffer->DefinitionPreview.has_value() && editorBuffer->DefinitionPreview->bCanApply && editorBuffer->bPreviewCurrent)
                : (editorBuffer->RelationPreview.has_value() && editorBuffer->RelationPreview->bCanApply && editorBuffer->bPreviewCurrent);
            ImGui::BeginDisabled(!bCanApply);
            if (ImGui::Button("Apply")) {
                FModelChangeRecordV2 changeRecord;
                if (ApplyEditorBufferV2(model, *editorBuffer, EModelChangeOriginV2::DirectEdit, &changeRecord)) {
                    ModelLastChangeRecord = changeRecord;
                    bModelHasLastChangeRecord = true;
                    ModelEditorStatus = "[Ok] Applied canonical update to " + changeRecord.ObjectId + ".";
                } else {
                    ModelEditorStatus = "[Error] Apply failed for " + editorBuffer->TargetId + ".";
                }
            }
            AddTooltipForLastItem(
                "Commit the current draft into the canonical selected object. This only succeeds after a fresh preview parses and passes required validation.",
                ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled);
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(!editorBuffer->bDraftDirty && !editorBuffer->bPreviewCurrent);
            if (ImGui::Button("Revert / Cancel")) {
                if (RevertEditorBufferV2(model, *editorBuffer)) {
                    ModelEditorStatus = "[Ok] Restored canonical notation for " + editorBuffer->TargetId + ".";
                }
            }
            AddTooltipForLastItem(
                "Discard the current draft and preview, then restore the editor from the canonical stored notation for the selected object.",
                ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled);
            ImGui::EndDisabled();

            if (!editorBuffer->bPreviewCurrent) {
                ImGui::TextDisabled("Preview the current draft before applying.");
                AddTooltipForLastItem("Apply is intentionally gated behind an explicit preview so canonical state never changes silently while you type.");
            } else if (editorBuffer->TargetKind == EModelObjectKindV2::Definition && editorBuffer->DefinitionPreview.has_value()) {
                const auto &preview = *editorBuffer->DefinitionPreview;
                if (!preview.bParseOk && preview.ParseError.has_value()) {
                    DrawDiagnosticList(preview.Diagnostics);
                } else {
                    Typesetting::ImGuiTypesetLabelValue(
                        typesettingService,
                        "Normalized",
                        MakeMathRequest(preview.NormalizedProjection, baseFontSize),
                        inspectorTheme.SecondaryTextOptions,
                        inspectorTheme.DetailMathOptions);
                    AddTooltipForLastItem("Canonicalized pretty projection of the parsed draft. This shows what the editor understood, not yet what has been committed.");
                    if (preview.InferredKind.has_value()) {
                        ImGui::BulletText("Inferred object/category: %s", ToString(*preview.InferredKind));
                        AddTooltipForLastItem("Best-effort semantic classification inferred from the draft and current model context.");
                    }
                    if (!preview.NormalizedInterpretation.empty()) {
                        ImGui::TextWrapped("%s", preview.NormalizedInterpretation.c_str());
                        AddTooltipForLastItem("Human-readable semantic summary of the parsed draft.");
                    }
                    DrawReferencedSymbols(preview.ReferencedSymbols);
                    DrawDiagnosticList(preview.Diagnostics);
                    DrawAssumptionSummary(preview.Assumptions);
                }
            } else if (editorBuffer->TargetKind == EModelObjectKindV2::Relation && editorBuffer->RelationPreview.has_value()) {
                const auto &preview = *editorBuffer->RelationPreview;
                if (!preview.bParseOk && preview.ParseError.has_value()) {
                    DrawDiagnosticList(preview.Diagnostics);
                } else {
                    Typesetting::ImGuiTypesetLabelValue(
                        typesettingService,
                        "Normalized",
                        MakeMathRequest(preview.NormalizedProjection, baseFontSize),
                        inspectorTheme.SecondaryTextOptions,
                        inspectorTheme.DetailMathOptions);
                    AddTooltipForLastItem("Canonicalized pretty projection of the parsed draft. This shows what the editor understood, not yet what has been committed.");
                    ImGui::BulletText("Inferred object/category: %s", ToString(preview.InferredClass));
                    AddTooltipForLastItem("Best-effort relation class inferred from derivative and operator structure in the draft.");
                    if (!preview.NormalizedInterpretation.empty()) {
                        ImGui::TextWrapped("%s", preview.NormalizedInterpretation.c_str());
                        AddTooltipForLastItem("Human-readable semantic summary of the parsed draft.");
                    }
                    DrawReferencedSymbols(preview.ReferencedSymbols);
                    DrawDiagnosticList(preview.Diagnostics);
                    DrawAssumptionSummary(preview.Assumptions);
                }
            }
        }
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("ModelAssumptionsPane", ImVec2(0.0f, 0.0f), true);
        ImGui::SeparatorText("Assumed / Inferred Semantics");
        AddTooltipForLastItem("Semantic proposals inferred from notation and context. They remain advisory until explicitly accepted or dismissed.");
        if (semanticReport.Assumptions.empty()) {
            ImGui::TextDisabled("No implicit or accepted semantics are currently available.");
            AddTooltipForLastItem("No current drafts or canonical relations produced semantic proposals beyond the already declared model objects.");
        } else {
            for (const auto &assumption : semanticReport.Assumptions) {
                ImGui::PushID(assumption.AssumptionId.c_str());
                const bool bSelected = SelectedModelAssumptionId == assumption.AssumptionId;
                if (ImGui::Selectable(
                        (assumption.Category + "##" + assumption.AssumptionId).c_str(),
                        bSelected,
                        ImGuiSelectableFlags_AllowItemOverlap)) {
                    SelectedModelAssumptionId = assumption.AssumptionId;
                }
                AddTooltipForLastItem("Select this inferred semantic proposal to inspect its target, source, role comparison, and available actions.");
                ImGui::TextDisabled(
                    "%s | target: %s | source: %s | status: %s",
                    ToString(assumption.Kind),
                    assumption.TargetSymbol.empty() ? assumption.TargetId.c_str() : assumption.TargetSymbol.c_str(),
                    assumption.SourceId.c_str(),
                    ToString(assumption.Status));
                AddTooltipForLastItem("Kind says what was inferred, target says what object or symbol it applies to, source says where it came from, and status records whether it stays implicit, was accepted, or was dismissed.");
                if (!assumption.Detail.empty()) {
                    ImGui::TextWrapped("%s", assumption.Detail.c_str());
                    AddTooltipForLastItem("Explanation of the evidence behind this assumption.");
                }
                if (assumption.DeclaredKind.has_value() || assumption.InferredKind.has_value()) {
                    ImGui::TextDisabled(
                        "Declared: %s | Inferred: %s",
                        assumption.DeclaredKind.has_value() ? ToString(*assumption.DeclaredKind) : "-",
                        assumption.InferredKind.has_value() ? ToString(*assumption.InferredKind) : "-");
                    AddTooltipForLastItem("Compares canonical declared role against inferred role. Mismatch is surfaced for trustworthiness instead of being auto-corrected.");
                }
                if (!assumption.ProposedArgumentDefinitionIds.empty()) {
                    Slab::Str dependencySummary;
                    for (std::size_t i = 0; i < assumption.ProposedArgumentDefinitionIds.size(); ++i) {
                        if (i != 0) dependencySummary += ", ";
                        dependencySummary += assumption.ProposedArgumentDefinitionIds[i];
                    }
                    ImGui::TextDisabled("Proposed dependency: %s", dependencySummary.c_str());
                    AddTooltipForLastItem("Dependencies inferred from usage, such as time dependence exposed by derivatives.");
                }
                if (!assumption.MaterializedDefinitionId.empty()) {
                    ImGui::TextDisabled("Materialized definition: %s", assumption.MaterializedDefinitionId.c_str());
                    AddTooltipForLastItem("This assumption has already been materialized into an explicit canonical Definition.");
                }

                const auto pushAssumptionStatusChange = [&](const EAssumptionStatusV2 from,
                                                            const EAssumptionStatusV2 to) {
                    FModelChangeRecordV2 changeRecord;
                    changeRecord.ObjectKind = EModelObjectKindV2::Assumption;
                    changeRecord.ObjectId = assumption.AssumptionId;
                    changeRecord.PreviousCanonicalNotation = ToString(from);
                    changeRecord.NewCanonicalNotation = ToString(to);
                    changeRecord.Origin = EModelChangeOriginV2::AcceptedInference;
                    changeRecord.SourceId = assumption.SourceId;
                    ApplyModelChangeRecordV2(model, changeRecord);
                    ModelLastChangeRecord = model.ChangeLog.back();
                    bModelHasLastChangeRecord = true;
                };

                if (assumption.Status != EAssumptionStatusV2::Accepted) {
                    if (ImGui::Button("Accept")) {
                        FModelChangeRecordV2 changeRecord;
                        if (AcceptAssumptionV2(model, assumption, &changeRecord)) {
                            ModelLastChangeRecord = changeRecord;
                            bModelHasLastChangeRecord = true;
                            ModelEditorStatus = "[Ok] Accepted semantic assumption '" + assumption.AssumptionId + "'.";
                            if (!changeRecord.ObjectId.empty() &&
                                changeRecord.ObjectKind == EModelObjectKindV2::Definition) {
                                SelectedModelDefinitionId = changeRecord.ObjectId;
                                bSelectedModelDetailIsRelation = false;
                            }
                        }
                    }
                    AddTooltipForLastItem("Accept this proposal. Undeclared inferred symbols may be materialized into explicit canonical Definitions; otherwise the assumption is recorded as accepted.");
                    ImGui::SameLine();
                }

                const bool bCanKeepImplicit = assumption.Status != EAssumptionStatusV2::Implicit &&
                    assumption.MaterializedDefinitionId.empty();
                ImGui::BeginDisabled(!bCanKeepImplicit);
                if (ImGui::Button("Keep implicit")) {
                    const auto previousStatus = assumption.Status;
                    if (SetAssumptionStatusV2(model, assumption.AssumptionId, EAssumptionStatusV2::Implicit)) {
                        pushAssumptionStatusChange(previousStatus, EAssumptionStatusV2::Implicit);
                        ModelEditorStatus = "[Ok] Kept assumption '" + assumption.AssumptionId + "' implicit.";
                    }
                }
                AddTooltipForLastItem(
                    "Leave this semantic proposal visible but non-canonical. Use this when the inference is helpful context but should not become an explicit declaration yet.",
                    ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled);
                ImGui::EndDisabled();
                ImGui::SameLine();

                const bool bCanDismiss = assumption.Status != EAssumptionStatusV2::Dismissed &&
                    assumption.MaterializedDefinitionId.empty();
                ImGui::BeginDisabled(!bCanDismiss);
                if (ImGui::Button("Dismiss")) {
                    const auto previousStatus = assumption.Status;
                    if (SetAssumptionStatusV2(model, assumption.AssumptionId, EAssumptionStatusV2::Dismissed)) {
                        pushAssumptionStatusChange(previousStatus, EAssumptionStatusV2::Dismissed);
                        ModelEditorStatus = "[Ok] Dismissed assumption '" + assumption.AssumptionId + "'.";
                    }
                }
                AddTooltipForLastItem(
                    "Hide this proposal from the assumptions surface without rewriting canonical Definitions or Relations. This is useful when the inference is not wanted.",
                    ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled);
                ImGui::EndDisabled();
                ImGui::Separator();
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }
    ImGui::EndChild();
}
