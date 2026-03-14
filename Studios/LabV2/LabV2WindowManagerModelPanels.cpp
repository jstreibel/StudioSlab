#include "LabV2WindowManager.h"

#include "Core/Model/V2/ModelRealizationV2.h"
#include "Core/Model/V2/ModelSeedsV2.h"
#include "Graphics/Typesetting/ImGuiTypesetting.h"
#include "imgui.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>

namespace {

    namespace ModelV2 = Slab::Core::Model::V2;
    namespace Typesetting = Slab::Graphics::Typesetting;

    struct FModelInspectorTheme {
        float BaseFontSize = 0.0f;
        Typesetting::FImGuiTypesetDrawOptions SectionTextOptions;
        Typesetting::FImGuiTypesetDrawOptions SecondaryTextOptions;
        Typesetting::FImGuiTypesetDrawOptions DetailMathOptions;
    };

    struct FSemanticRowHighlightState {
        bool bSelected = false;
        bool bDependsOn = false;
        bool bAmbientDependency = false;
        bool bUsedBy = false;
        bool bRelatedAssumption = false;
        bool bSource = false;
        bool bTarget = false;
        bool bConflict = false;
        bool bOverride = false;

        [[nodiscard]] auto HasContext() const -> bool {
            return bDependsOn || bAmbientDependency || bUsedBy || bRelatedAssumption ||
                bSource || bTarget || bConflict || bOverride;
        }
    };

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

    auto FindResolvedVocabularyEntry(const Slab::Vector<ModelV2::FResolvedBaseVocabularyEntryV2> &entries,
                                     const Slab::Str &entryId) -> const ModelV2::FResolvedBaseVocabularyEntryV2 * {
        const auto it = std::find_if(entries.begin(), entries.end(), [&](const auto &entry) {
            return entry.Entry.EntryId == entryId;
        });
        if (it == entries.end()) return nullptr;
        return &(*it);
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

    auto FindAssumptionReport(const ModelV2::FModelSemanticReportV2 &report,
                              const Slab::Str &assumptionId) -> const ModelV2::FSemanticAssumptionV2 * {
        const auto it = std::find_if(report.Assumptions.begin(), report.Assumptions.end(), [&](const auto &entry) {
            return entry.AssumptionId == assumptionId;
        });
        if (it == report.Assumptions.end()) return nullptr;
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

    auto GetEditorBufferPreviewOverview(const ModelV2::FModelEditorBufferV2 *buffer) -> const ModelV2::FModelSemanticOverviewV2 * {
        if (buffer == nullptr || !buffer->bPreviewCurrent) return nullptr;
        if (buffer->TargetKind == ModelV2::EModelObjectKindV2::Definition &&
            buffer->DefinitionPreview.has_value() &&
            buffer->DefinitionPreview->SemanticOverview.has_value()) {
            return &(*buffer->DefinitionPreview->SemanticOverview);
        }
        if (buffer->TargetKind == ModelV2::EModelObjectKindV2::Relation &&
            buffer->RelationPreview.has_value() &&
            buffer->RelationPreview->SemanticOverview.has_value()) {
            return &(*buffer->RelationPreview->SemanticOverview);
        }
        return nullptr;
    }

    auto GetEditorBufferPreviewAssumptions(const ModelV2::FModelEditorBufferV2 *buffer)
        -> const Slab::Vector<ModelV2::FSemanticAssumptionV2> * {
        if (buffer == nullptr || !buffer->bPreviewCurrent) return nullptr;
        if (buffer->TargetKind == ModelV2::EModelObjectKindV2::Definition &&
            buffer->DefinitionPreview.has_value()) {
            return &buffer->DefinitionPreview->Assumptions;
        }
        if (buffer->TargetKind == ModelV2::EModelObjectKindV2::Relation &&
            buffer->RelationPreview.has_value()) {
            return &buffer->RelationPreview->Assumptions;
        }
        return nullptr;
    }

    auto FindDraftProposedDefinition(const ModelV2::FModelV2 &model,
                                     const ModelV2::FModelEditorBufferV2 *buffer,
                                     const ModelV2::FModelSemanticOverviewV2 *previewOverview,
                                     const Slab::Str &definitionId) -> Slab::TOptional<ModelV2::FDefinitionV2> {
        const auto *assumptions = GetEditorBufferPreviewAssumptions(buffer);
        if (assumptions == nullptr) return std::nullopt;

        for (const auto &assumption : *assumptions) {
            const auto materializationPreview = ModelV2::BuildAssumptionMaterializationPreviewV2(model, assumption, previewOverview);
            if (!materializationPreview.ProposedDefinition.has_value()) continue;
            if (materializationPreview.ProposedDefinition->DefinitionId == definitionId) {
                return materializationPreview.ProposedDefinition;
            }
        }

        return std::nullopt;
    }

    auto CollectDraftProposedDefinitions(const ModelV2::FModelV2 &model,
                                         const ModelV2::FModelEditorBufferV2 *buffer,
                                         const ModelV2::FModelSemanticOverviewV2 *previewOverview)
        -> Slab::Vector<ModelV2::FDefinitionV2> {
        Slab::Vector<ModelV2::FDefinitionV2> definitions;
        const auto *assumptions = GetEditorBufferPreviewAssumptions(buffer);
        if (assumptions == nullptr) return definitions;

        std::set<Slab::Str> definitionIds;
        for (const auto &assumption : *assumptions) {
            const auto materializationPreview = ModelV2::BuildAssumptionMaterializationPreviewV2(model, assumption, previewOverview);
            if (!materializationPreview.ProposedDefinition.has_value()) continue;
            if (!definitionIds.insert(materializationPreview.ProposedDefinition->DefinitionId).second) continue;
            definitions.push_back(*materializationPreview.ProposedDefinition);
        }
        return definitions;
    }

    auto ResolveHighlightState(const ModelV2::FSemanticSelectionContextV2 &selectionContext,
                               const ModelV2::FModelSemanticOverviewV2 &overview,
                               const ModelV2::FSemanticObjectRefV2 &ref) -> FSemanticRowHighlightState {
        FSemanticRowHighlightState state;
        state.bSelected = ModelV2::AreSemanticObjectRefsEqualV2(selectionContext.Selected, ref);
        const auto key = ModelV2::MakeSemanticObjectKeyV2(ref);
        state.bDependsOn = selectionContext.DependencyKeys.contains(key);
        state.bAmbientDependency = selectionContext.AmbientDependencyKeys.contains(key);
        state.bUsedBy = selectionContext.UsedByKeys.contains(key);
        state.bRelatedAssumption = selectionContext.RelatedAssumptionKeys.contains(key);
        state.bSource = selectionContext.SourceKeys.contains(key);
        state.bTarget = selectionContext.TargetKeys.contains(key);
        state.bOverride = selectionContext.OverrideKeys.contains(key);
        state.bConflict = selectionContext.ConflictKeys.contains(key);

        if (const auto *object = overview.FindObject(ref); object != nullptr) {
            state.bConflict = state.bConflict || object->bConflict;
            state.bOverride = state.bOverride || object->bLocalOverride;
        }

        return state;
    }

    auto AppendBadge(Slab::Str &summary, const char *label) -> void {
        if (label == nullptr || label[0] == '\0') return;
        if (!summary.empty()) summary += " | ";
        summary += label;
    }

    auto BuildHighlightBadgeSummary(const FSemanticRowHighlightState &state) -> Slab::Str {
        Slab::Str summary;
        if (state.bDependsOn) AppendBadge(summary, "depends on");
        if (state.bAmbientDependency) AppendBadge(summary, "ambient");
        if (state.bUsedBy) AppendBadge(summary, "used by");
        if (state.bRelatedAssumption) AppendBadge(summary, "assumption");
        if (state.bSource) AppendBadge(summary, "source");
        if (state.bTarget) AppendBadge(summary, "target");
        if (state.bOverride) AppendBadge(summary, "override");
        if (state.bConflict) AppendBadge(summary, "conflict");
        return summary;
    }

    auto MakeContextualRowOptions(const ImVec4 &accentColor,
                                  const FSemanticRowHighlightState &highlightState,
                                  const float baseFontSize) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        Typesetting::FImGuiSelectableTypesetRowOptions rowOptions;
        rowOptions.bSelected = highlightState.bSelected;
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

        const auto badges = BuildHighlightBadgeSummary(highlightState);
        if (!badges.empty()) {
            rowOptions.SecondaryText = badges;
            rowOptions.SecondaryStyle.FontPixelSize = baseFontSize * 0.76f;
            rowOptions.InnerSpacing = 2.0f;
            rowOptions.Padding = ImVec2(8.0f, 5.0f);
            rowOptions.SecondaryTint = highlightState.bConflict
                ? ImGui::GetColorU32(ImVec4(0.86f, 0.47f, 0.26f, 1.0f))
                : ImGui::GetColorU32(ImGuiCol_TextDisabled);
        }

        if (!highlightState.bSelected && highlightState.HasContext()) {
            ImVec4 contextColor = accentColor;
            if (highlightState.bAmbientDependency) contextColor = ImVec4(0.72f, 0.58f, 0.24f, 1.0f);
            if (highlightState.bConflict) contextColor = ImVec4(0.86f, 0.47f, 0.26f, 1.0f);
            rowOptions.BackgroundTint = ImGui::GetColorU32(ImVec4(contextColor.x, contextColor.y, contextColor.z, 0.08f));
            rowOptions.BorderTint = ImGui::GetColorU32(ImVec4(contextColor.x, contextColor.y, contextColor.z, 0.36f));
            rowOptions.AccentTint = ImGui::GetColorU32(ImVec4(contextColor.x, contextColor.y, contextColor.z, 0.90f));
        }

        return rowOptions;
    }

    auto MakeDefinitionRowOptions(const FSemanticRowHighlightState &highlightState,
                                  const float baseFontSize) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        return MakeContextualRowOptions(ImVec4(0.29f, 0.60f, 0.45f, 1.0f), highlightState, baseFontSize);
    }

    auto MakeRelationRowOptions(const FSemanticRowHighlightState &highlightState,
                                const float baseFontSize) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        return MakeContextualRowOptions(ImVec4(0.31f, 0.49f, 0.74f, 1.0f), highlightState, baseFontSize);
    }

    auto MakeVocabularyRowOptions(const FSemanticRowHighlightState &highlightState,
                                  const float baseFontSize) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        auto rowOptions = MakeContextualRowOptions(ImVec4(0.72f, 0.58f, 0.24f, 1.0f), highlightState, baseFontSize);
        rowOptions.Padding = ImVec2(10.0f, 8.0f);
        rowOptions.InnerSpacing = 3.0f;
        return rowOptions;
    }

    auto MakeAssumptionRowOptions(const FSemanticRowHighlightState &highlightState,
                                  const float baseFontSize) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        return MakeContextualRowOptions(ImVec4(0.79f, 0.42f, 0.20f, 1.0f), highlightState, baseFontSize);
    }

    auto MakeDraftDefinitionRowOptions(const FSemanticRowHighlightState &highlightState,
                                       const float baseFontSize) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        auto rowOptions = MakeDefinitionRowOptions(highlightState, baseFontSize);
        rowOptions.SecondaryText = "draft only";
        rowOptions.SecondaryStyle.FontPixelSize = baseFontSize * 0.76f;
        rowOptions.SecondaryTint = ImGui::GetColorU32(ImVec4(0.76f, 0.60f, 0.22f, 0.95f));
        return rowOptions;
    }

    auto MakeDraftAssumptionRowOptions(const FSemanticRowHighlightState &highlightState,
                                       const float baseFontSize) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        auto rowOptions = MakeAssumptionRowOptions(highlightState, baseFontSize);
        rowOptions.SecondaryText = "draft preview";
        rowOptions.SecondaryStyle.FontPixelSize = baseFontSize * 0.76f;
        rowOptions.SecondaryTint = ImGui::GetColorU32(ImVec4(0.76f, 0.60f, 0.22f, 0.95f));
        return rowOptions;
    }

    auto HealthTint(const ModelV2::ESemanticHealthV2 health) -> ImVec4 {
        switch (health) {
            case ModelV2::ESemanticHealthV2::Ok:
                return ImVec4(0.33f, 0.67f, 0.45f, 1.0f);
            case ModelV2::ESemanticHealthV2::Deferred:
                return ImVec4(0.69f, 0.59f, 0.25f, 1.0f);
            case ModelV2::ESemanticHealthV2::Warning:
                return ImVec4(0.86f, 0.55f, 0.22f, 1.0f);
            case ModelV2::ESemanticHealthV2::Error:
                return ImVec4(0.82f, 0.28f, 0.28f, 1.0f);
        }

        return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    }

    auto DrawSemanticSummaryBadge(const char *label,
                                  const Slab::Str &value,
                                  const ImVec4 tint) -> void {
        if (value.empty()) return;
        ImGui::PushStyleColor(ImGuiCol_Text, tint);
        ImGui::Text("%s: %s", label, value.c_str());
        ImGui::PopStyleColor();
    }

    template<typename FNavigate>
    auto DrawSummaryLinkStrip(const char *label,
                              const Slab::Vector<ModelV2::FSemanticNavigationLinkV2> &links,
                              const FNavigate &navigate) -> void {
        if (links.empty()) return;
        ImGui::PushID(label);
        ImGui::TextDisabled("%s:", label);
        ImGui::SameLine();
        for (std::size_t i = 0; i < links.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::SmallButton(links[i].Label.c_str())) {
                navigate(links[i].Target);
            }
            if (!links[i].Detail.empty()) {
                AddTooltipForLastItem(links[i].Detail);
            }
            ImGui::PopID();
            if (i + 1 < links.size()) ImGui::SameLine();
        }
        ImGui::PopID();
    }

    auto BuildLinkBadgeSummary(const ModelV2::FSemanticNavigationLinkV2 &link) -> Slab::Str {
        Slab::Str summary;
        if (link.bAmbient) AppendBadge(summary, "ambient");
        if (link.bReadonly) AppendBadge(summary, "readonly");
        if (link.bOverride) AppendBadge(summary, "override");
        if (link.bConflict) AppendBadge(summary, "conflict");
        return summary;
    }

    template<typename FNavigate>
    auto DrawSemanticLinkSection(const char *title,
                                 const Slab::Vector<ModelV2::FSemanticNavigationLinkV2> &links,
                                 const FNavigate &navigate) -> void {
        if (links.empty()) return;
        ImGui::SeparatorText(title);
        for (std::size_t i = 0; i < links.size(); ++i) {
            const auto &link = links[i];
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::SmallButton(link.Label.c_str())) {
                navigate(link.Target);
            }
            if (!link.Detail.empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("%s", link.Detail.c_str());
            }
            const auto badges = BuildLinkBadgeSummary(link);
            if (!badges.empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("[%s]", badges.c_str());
            }
            ImGui::PopID();
        }
    }

    template<typename FNavigate>
    auto DrawDiagnosticNavigationSection(const Slab::Vector<ModelV2::FSemanticDiagnosticNavigationV2> &diagnostics,
                                         const FNavigate &navigate) -> void {
        if (diagnostics.empty()) return;
        ImGui::SeparatorText("Diagnostics");
        for (std::size_t i = 0; i < diagnostics.size(); ++i) {
            const auto &diagnostic = diagnostics[i];
            const auto tint = diagnostic.Diagnostic.Severity == ModelV2::EValidationSeverityV2::Error
                ? ImVec4(0.82f, 0.26f, 0.26f, 1.0f)
                : ImVec4(0.87f, 0.67f, 0.22f, 1.0f);
            ImGui::PushID(static_cast<int>(i));
            if (diagnostic.NavigateTo.has_value()) {
                if (ImGui::SmallButton("Jump")) {
                    navigate(*diagnostic.NavigateTo);
                }
                ImGui::SameLine();
            }
            ImGui::PushStyleColor(ImGuiCol_Text, tint);
            ImGui::TextWrapped(
                "[%s] %s: %s",
                diagnostic.Diagnostic.Code.c_str(),
                diagnostic.Diagnostic.Context.c_str(),
                diagnostic.Diagnostic.Message.c_str());
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
    }

    auto DrawRawDiagnosticList(const Slab::Vector<ModelV2::FSemanticDiagnosticV2> &diagnostics) -> void;

    template<typename FNavigate>
    auto DrawPreviewDiagnosticSection(const ModelV2::FModelSemanticOverviewV2 *overview,
                                      const ModelV2::FSemanticObjectRefV2 &objectRef,
                                      const Slab::Vector<ModelV2::FSemanticDiagnosticV2> &fallbackDiagnostics,
                                      const FNavigate &navigate) -> void {
        if (overview != nullptr) {
            if (const auto *object = overview->FindObject(objectRef); object != nullptr && !object->Diagnostics.empty()) {
                DrawDiagnosticNavigationSection(object->Diagnostics, navigate);
                return;
            }
        }

        DrawRawDiagnosticList(fallbackDiagnostics);
    }

    template<typename FNavigate, typename FResolve>
    auto DrawPreviewDiagnosticSectionWithResolution(
        const ModelV2::FModelSemanticOverviewV2 *overview,
        const ModelV2::FSemanticObjectRefV2 &objectRef,
        const Slab::Vector<ModelV2::FSemanticDiagnosticV2> &fallbackDiagnostics,
        const Slab::Vector<ModelV2::FReferencedSymbolSemanticV2> &referencedSymbols,
        const Slab::Vector<ModelV2::FSemanticAssumptionV2> *assumptions,
        const FNavigate &navigate,
        const FResolve &resolveSymbol) -> bool {
        const ModelV2::FSemanticObjectOverviewV2 *object = nullptr;
        if (overview != nullptr) object = overview->FindObject(objectRef);

        const bool bUseObjectDiagnostics = object != nullptr && !object->Diagnostics.empty();
        if (!bUseObjectDiagnostics && fallbackDiagnostics.empty()) return false;

        bool bChanged = false;
        ImGui::SeparatorText("Diagnostics");

        const auto renderDiagnosticRow = [&](const ModelV2::FSemanticDiagnosticV2 &diagnostic,
                                             const Slab::TOptional<ModelV2::FSemanticObjectRefV2> &navigateTo) {
            const auto tint = diagnostic.Severity == ModelV2::EValidationSeverityV2::Error
                ? ImVec4(0.82f, 0.26f, 0.26f, 1.0f)
                : ImVec4(0.87f, 0.67f, 0.22f, 1.0f);

            ImGui::PushID((diagnostic.Code + "|" + diagnostic.Context + "|" + diagnostic.Message).c_str());
            if (navigateTo.has_value()) {
                if (ImGui::SmallButton("Go")) {
                    navigate(*navigateTo);
                }
                ImGui::SameLine();
            }

            if (diagnostic.Code == "unresolved_symbol") {
                if (const auto *symbol = ModelV2::Detail::FindReferencedSymbolForDiagnosticV2(referencedSymbols, diagnostic);
                    symbol != nullptr) {
                    const auto *assumption = assumptions != nullptr
                        ? ModelV2::Detail::FindAssumptionForReferencedSymbolV2(*assumptions, *symbol)
                        : nullptr;
                    if (ImGui::SmallButton("Resolve")) {
                        if (resolveSymbol(*symbol, assumption)) bChanged = true;
                    }
                    ImGui::SameLine();
                }
            }

            ImGui::PushStyleColor(ImGuiCol_Text, tint);
            ImGui::TextWrapped("[%s] %s: %s", diagnostic.Code.c_str(), diagnostic.Context.c_str(), diagnostic.Message.c_str());
            ImGui::PopStyleColor();
            ImGui::PopID();
        };

        if (bUseObjectDiagnostics) {
            for (const auto &navigation : object->Diagnostics) {
                renderDiagnosticRow(navigation.Diagnostic, navigation.NavigateTo);
            }
        } else {
            for (const auto &diagnostic : fallbackDiagnostics) {
                renderDiagnosticRow(diagnostic, {});
            }
        }

        return bChanged;
    }

    auto DrawRawDiagnosticList(const Slab::Vector<ModelV2::FSemanticDiagnosticV2> &diagnostics) -> void {
        if (diagnostics.empty()) return;
        ImGui::SeparatorText("Diagnostics");
        for (const auto &diagnostic : diagnostics) {
            const auto tint = diagnostic.Severity == ModelV2::EValidationSeverityV2::Error
                ? ImVec4(0.82f, 0.26f, 0.26f, 1.0f)
                : ImVec4(0.87f, 0.67f, 0.22f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, tint);
            ImGui::BulletText("[%s] %s: %s", diagnostic.Code.c_str(), diagnostic.Context.c_str(), diagnostic.Message.c_str());
            ImGui::PopStyleColor();
        }
    }

    template<typename FNavigate, typename FResolve>
    auto DrawReferencedSymbols(const ModelV2::FModelSemanticOverviewV2 *overview,
                               const Slab::Vector<ModelV2::FReferencedSymbolSemanticV2> &symbols,
                               const Slab::Vector<ModelV2::FSemanticAssumptionV2> *assumptions,
                               const FNavigate &navigate,
                               const FResolve &resolveSymbol) -> bool {
        if (symbols.empty()) return false;
        bool bChanged = false;
        ImGui::SeparatorText("Referenced Symbols");
        for (const auto &symbol : symbols) {
            const auto navigationTarget =
                overview != nullptr ? ModelV2::ResolveReferencedSymbolNavigationTargetV2(*overview, symbol) : Slab::TOptional<ModelV2::FSemanticObjectRefV2>{};
            ImGui::PushID(symbol.SymbolText.c_str());
            if (navigationTarget.has_value()) {
                if (ImGui::SmallButton((symbol.SymbolText.empty() ? symbol.ReferenceId : symbol.SymbolText).c_str())) {
                    navigate(*navigationTarget);
                }
                ImGui::SameLine();
                ImGui::TextDisabled("%s", symbol.bResolved ? "resolved" : "unresolved");
            } else {
                ImGui::BulletText(
                    "%s | %s",
                    symbol.SymbolText.c_str(),
                    symbol.bResolved ? "resolved" : "unresolved");
            }
            if (!symbol.ReferenceId.empty()) {
                const auto label = symbol.Origin == ModelV2::ESemanticOriginV2::BaseVocabulary
                    ? "Vocabulary entry"
                    : "Definition";
                ImGui::TextDisabled("%s: %s", label, symbol.ReferenceId.c_str());
            }
            ImGui::TextDisabled("Origin: %s", ModelV2::ToString(symbol.Origin));
            if (symbol.Origin == ModelV2::ESemanticOriginV2::BaseVocabulary && !symbol.SourcePresetId.empty()) {
                ImGui::TextDisabled("Preset: %s", symbol.SourcePresetId.c_str());
            }
            if (symbol.VocabularyKind.has_value()) {
                ImGui::TextDisabled("Vocabulary kind: %s", ModelV2::ToString(*symbol.VocabularyKind));
            }
            if (!symbol.SemanticRoleSummary.empty()) {
                ImGui::TextWrapped("%s", symbol.SemanticRoleSummary.c_str());
            }
            if (!symbol.UsageLabels.empty()) {
                Slab::Str usageSummary;
                for (std::size_t i = 0; i < symbol.UsageLabels.size(); ++i) {
                    if (i != 0) usageSummary += ", ";
                    usageSummary += symbol.UsageLabels[i];
                }
                ImGui::TextDisabled("Uses: %s", usageSummary.c_str());
            }
            if (symbol.DeclaredKind.has_value()) {
                ImGui::TextDisabled("Declared: %s", ModelV2::ToString(*symbol.DeclaredKind));
            }
            if (symbol.InferredKind.has_value()) {
                ImGui::TextDisabled("Inferred: %s", ModelV2::ToString(*symbol.InferredKind));
            }
            if (!symbol.bResolved) {
                const auto *assumption = assumptions != nullptr
                    ? ModelV2::Detail::FindAssumptionForReferencedSymbolV2(*assumptions, symbol)
                    : nullptr;
                if (ImGui::SmallButton(assumption != nullptr ? "Resolve / Materialize" : "Resolve / Define")) {
                    if (resolveSymbol(symbol, assumption)) bChanged = true;
                }
            }
            ImGui::PopID();
        }
        return bChanged;
    }

    template<typename FNavigate, typename FAccept>
    auto DrawDraftAssumptionWorkflowSummary(ModelV2::FModelV2 &model,
                                            const Slab::Vector<ModelV2::FSemanticAssumptionV2> &assumptions,
                                            const ModelV2::FModelSemanticOverviewV2 *previewOverview,
                                            const FNavigate &navigate,
                                            const FAccept &acceptAssumption) -> bool {
        if (assumptions.empty()) return false;
        ImGui::SeparatorText("Assumption Workflow");
        for (const auto &assumption : assumptions) {
            const auto materializationPreview = ModelV2::BuildAssumptionMaterializationPreviewV2(model, assumption, previewOverview);
            ImGui::PushID(assumption.AssumptionId.c_str());
            const auto assumptionLabel = assumption.TargetSymbol.empty() ? assumption.TargetId : assumption.TargetSymbol;
            if (ImGui::SmallButton(assumptionLabel.c_str())) {
                navigate(ModelV2::MakeAssumptionObjectRefV2(assumption.AssumptionId));
            }
            ImGui::SameLine();
            ImGui::TextDisabled("%s | %s", assumption.Category.c_str(), materializationPreview.LifecycleLabel.c_str());
            if (!assumption.Detail.empty()) {
                ImGui::TextWrapped("%s", assumption.Detail.c_str());
            }
            if (materializationPreview.ProposedDefinition.has_value()) {
                ImGui::TextDisabled("Would materialize as: %s", materializationPreview.SuggestedDefinitionId.c_str());
                ImGui::SameLine();
                if (ImGui::SmallButton("Inspect Draft Object")) {
                    navigate(ModelV2::MakeDefinitionObjectRefV2(materializationPreview.SuggestedDefinitionId));
                }
            }
            if (!materializationPreview.OutcomeLabel.empty()) {
                ImGui::TextWrapped("%s", materializationPreview.OutcomeLabel.c_str());
            }

            if (assumption.Status != ModelV2::EAssumptionStatusV2::Accepted) {
                const char *buttonLabel = materializationPreview.ProposedDefinition.has_value()
                    ? "Materialize into model"
                    : "Accept into model";
                if (ImGui::SmallButton(buttonLabel)) {
                    const bool bMutated = acceptAssumption(assumption, materializationPreview);
                    if (bMutated) {
                        ImGui::PopID();
                        return true;
                    }
                }
            }
            ImGui::PopID();
        }
        return false;
    }

    auto DeltaTint(const ModelV2::ESemanticDeltaKindV2 kind) -> ImVec4 {
        switch (kind) {
            case ModelV2::ESemanticDeltaKindV2::Added:
                return ImVec4(0.32f, 0.66f, 0.42f, 1.0f);
            case ModelV2::ESemanticDeltaKindV2::Removed:
                return ImVec4(0.80f, 0.33f, 0.31f, 1.0f);
            case ModelV2::ESemanticDeltaKindV2::Changed:
                return ImVec4(0.84f, 0.64f, 0.24f, 1.0f);
        }

        return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    }

    auto DeltaPrefix(const ModelV2::ESemanticDeltaKindV2 kind) -> const char * {
        switch (kind) {
            case ModelV2::ESemanticDeltaKindV2::Added: return "+";
            case ModelV2::ESemanticDeltaKindV2::Removed: return "-";
            case ModelV2::ESemanticDeltaKindV2::Changed: return "~";
        }

        return "~";
    }

    auto DrawDraftStatusDeltaLine(const char *label,
                                  const Slab::Str &canonicalValue,
                                  const Slab::Str &draftValue) -> void {
        const bool bChanged = canonicalValue != draftValue;
        if (!bChanged) {
            ImGui::TextDisabled("%s: %s", label, draftValue.c_str());
            return;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.84f, 0.64f, 0.24f, 1.0f));
        ImGui::TextWrapped("%s: %s -> %s", label, canonicalValue.c_str(), draftValue.c_str());
        ImGui::PopStyleColor();
    }

    auto JoinIds(const Slab::Vector<Slab::Str> &ids) -> Slab::Str {
        Slab::Str result;
        for (std::size_t i = 0; i < ids.size(); ++i) {
            if (i != 0) result += ", ";
            result += ids[i];
        }
        return result;
    }

    auto BuildReferencedSymbolDeltaSummary(const ModelV2::FReferencedSymbolDeltaV2 &delta) -> Slab::Str {
        if (delta.Kind == ModelV2::ESemanticDeltaKindV2::Added) {
            return delta.Symbol.bResolved
                ? "new referenced symbol now resolved"
                : "new referenced symbol remains unresolved";
        }
        if (delta.Kind == ModelV2::ESemanticDeltaKindV2::Removed) {
            return "reference removed from draft";
        }

        Slab::Str summary;
        if (delta.bResolutionChanged) {
            summary += delta.bCanonicalResolved ? "resolved -> unresolved" : "unresolved -> resolved";
        }
        if (delta.bOriginChanged) {
            if (!summary.empty()) summary += " | ";
            summary += Slab::Str("origin: ") + ModelV2::ToString(delta.CanonicalOrigin) +
                " -> " + ModelV2::ToString(delta.Symbol.Origin);
        }
        if (delta.bDeclaredKindChanged) {
            if (!summary.empty()) summary += " | ";
            summary += "declared role changed";
        }
        if (delta.bInferredKindChanged) {
            if (!summary.empty()) summary += " | ";
            summary += "inferred role changed";
        }
        if (delta.bArgumentDependenciesChanged) {
            if (!summary.empty()) summary += " | ";
            summary += "argument dependencies changed";
        }
        return summary.empty() ? "semantic reference changed" : summary;
    }

    template<typename FNavigate>
    auto DrawNavigationDeltaSection(const char *title,
                                    const Slab::Vector<ModelV2::FSemanticNavigationDeltaV2> &deltas,
                                    const FNavigate &navigate) -> void {
        if (deltas.empty()) return;
        ImGui::SeparatorText(title);
        for (std::size_t i = 0; i < deltas.size(); ++i) {
            const auto &delta = deltas[i];
            ImGui::PushID(static_cast<int>(i));
            ImGui::PushStyleColor(ImGuiCol_Text, DeltaTint(delta.Kind));
            ImGui::TextUnformatted(DeltaPrefix(delta.Kind));
            ImGui::PopStyleColor();
            ImGui::SameLine();
            if (delta.Link.Target.IsValid()) {
                if (ImGui::SmallButton(delta.Link.Label.c_str())) {
                    navigate(delta.Link.Target);
                }
            } else {
                ImGui::TextUnformatted(delta.Link.Label.c_str());
            }

            Slab::Str detail = delta.Kind == ModelV2::ESemanticDeltaKindV2::Changed &&
                    delta.PreviousDetail != delta.NewDetail && !delta.NewDetail.empty()
                ? delta.PreviousDetail + " -> " + delta.NewDetail
                : (!delta.NewDetail.empty() ? delta.NewDetail : delta.PreviousDetail);
            if (!detail.empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("%s", detail.c_str());
            }
            ImGui::PopID();
        }
    }

    auto DrawReferencedSymbolDeltaSection(const char *title,
                                          const Slab::Vector<ModelV2::FReferencedSymbolDeltaV2> &deltas) -> void {
        if (deltas.empty()) return;
        ImGui::SeparatorText(title);
        for (const auto &delta : deltas) {
            const auto label = delta.Symbol.SymbolText.empty() ? delta.Symbol.ReferenceId : delta.Symbol.SymbolText;
            ImGui::PushStyleColor(ImGuiCol_Text, DeltaTint(delta.Kind));
            ImGui::BulletText("%s %s", DeltaPrefix(delta.Kind), label.c_str());
            ImGui::PopStyleColor();
            ImGui::TextDisabled("%s", BuildReferencedSymbolDeltaSummary(delta).c_str());
            if (delta.Symbol.InferredKind.has_value()) {
                ImGui::TextDisabled("Draft inferred role: %s", ModelV2::ToString(*delta.Symbol.InferredKind));
            }
        }
    }

    auto BuildAssumptionDeltaSummary(const ModelV2::FSemanticAssumptionDeltaV2 &delta) -> Slab::Str {
        if (delta.Kind == ModelV2::ESemanticDeltaKindV2::Added) {
            return delta.Assumption.bWouldCreateDefinition
                ? "new semantic proposal would materialize as a local definition"
                : "new semantic proposal";
        }
        if (delta.Kind == ModelV2::ESemanticDeltaKindV2::Removed) {
            return "proposal removed by draft";
        }

        Slab::Str summary;
        if (delta.bStatusChanged) {
            summary += Slab::Str("status: ") + ModelV2::ToString(delta.CanonicalStatus) +
                " -> " + ModelV2::ToString(delta.Assumption.Status);
        }
        if (delta.bMaterializationChanged) {
            if (!summary.empty()) summary += " | ";
            summary += "materialization changed";
        }
        if (summary.empty()) summary = "proposal meaning changed";
        return summary;
    }

    template<typename FNavigate>
    auto DrawAssumptionDeltaSection(const char *title,
                                    const Slab::Vector<ModelV2::FSemanticAssumptionDeltaV2> &deltas,
                                    const FNavigate &navigate) -> void {
        if (deltas.empty()) return;
        ImGui::SeparatorText(title);
        for (std::size_t i = 0; i < deltas.size(); ++i) {
            const auto &delta = deltas[i];
            const auto label = delta.Assumption.TargetSymbol.empty()
                ? (delta.Assumption.TargetId.empty() ? delta.Assumption.AssumptionId : delta.Assumption.TargetId)
                : delta.Assumption.TargetSymbol;
            ImGui::PushID(static_cast<int>(i));
            ImGui::PushStyleColor(ImGuiCol_Text, DeltaTint(delta.Kind));
            ImGui::TextUnformatted(DeltaPrefix(delta.Kind));
            ImGui::PopStyleColor();
            ImGui::SameLine();
            if (ImGui::SmallButton(label.c_str())) {
                navigate(ModelV2::MakeAssumptionObjectRefV2(delta.Assumption.AssumptionId));
            }
            ImGui::SameLine();
            ImGui::TextDisabled("%s", delta.Assumption.Category.c_str());
            ImGui::TextDisabled("%s", BuildAssumptionDeltaSummary(delta).c_str());
            if (!delta.Assumption.Detail.empty()) {
                ImGui::TextWrapped("%s", delta.Assumption.Detail.c_str());
            }
            ImGui::PopID();
        }
    }

    auto DrawDiagnosticDeltaSection(const char *title,
                                    const Slab::Vector<ModelV2::FSemanticDiagnosticDeltaV2> &deltas) -> void {
        if (deltas.empty()) return;
        ImGui::SeparatorText(title);
        for (const auto &delta : deltas) {
            const auto tint = delta.Kind == ModelV2::ESemanticDeltaKindV2::Removed
                ? ImVec4(0.32f, 0.66f, 0.42f, 1.0f)
                : DeltaTint(delta.Kind);
            ImGui::PushStyleColor(ImGuiCol_Text, tint);
            ImGui::BulletText(
                "%s [%s] %s: %s",
                DeltaPrefix(delta.Kind),
                delta.Diagnostic.Code.c_str(),
                delta.Diagnostic.Context.c_str(),
                delta.Diagnostic.Message.c_str());
            ImGui::PopStyleColor();
        }
    }

    auto DraftDeltaHasChanges(const ModelV2::FModelSemanticDraftDeltaV2 &delta) -> bool {
        return delta.CanonicalStatus.ParseHealth != delta.DraftStatus.ParseHealth ||
            delta.CanonicalStatus.SemanticHealth != delta.DraftStatus.SemanticHealth ||
            delta.CanonicalStatus.UnresolvedSymbolCount != delta.DraftStatus.UnresolvedSymbolCount ||
            delta.CanonicalStatus.PendingAssumptionCount != delta.DraftStatus.PendingAssumptionCount ||
            delta.CanonicalStatus.DeferredCount != delta.DraftStatus.DeferredCount ||
            delta.CanonicalStatus.Classification.ModelClass != delta.DraftStatus.Classification.ModelClass ||
            delta.CanonicalStatus.Classification.Character != delta.DraftStatus.Classification.Character ||
            delta.CanonicalInterpretation != delta.DraftInterpretation ||
            delta.CanonicalInferredKind != delta.DraftInferredKind ||
            delta.CanonicalRelationClass != delta.DraftRelationClass ||
            delta.bCanonicalConflict != delta.bDraftConflict ||
            !delta.ReferencedSymbolChanges.empty() ||
            !delta.DependencyChanges.empty() ||
            !delta.AmbientDependencyChanges.empty() ||
            !delta.UsedByChanges.empty() ||
            !delta.AssumptionLinkChanges.empty() ||
            !delta.OverrideChanges.empty() ||
            !delta.AssumptionChanges.empty() ||
            !delta.DiagnosticChanges.empty();
    }

    template<typename FNavigate>
    auto DrawDraftSemanticDelta(const ModelV2::FModelSemanticDraftDeltaV2 &delta,
                                const FNavigate &navigate) -> void {
        ImGui::SeparatorText("Draft Semantic Delta");
        DrawDraftStatusDeltaLine(
            "Parse health",
            ModelV2::ToString(delta.CanonicalStatus.ParseHealth),
            ModelV2::ToString(delta.DraftStatus.ParseHealth));
        DrawDraftStatusDeltaLine(
            "Semantic health",
            ModelV2::ToString(delta.CanonicalStatus.SemanticHealth),
            ModelV2::ToString(delta.DraftStatus.SemanticHealth));
        DrawDraftStatusDeltaLine(
            "Unresolved symbols",
            std::to_string(delta.CanonicalStatus.UnresolvedSymbolCount),
            std::to_string(delta.DraftStatus.UnresolvedSymbolCount));
        DrawDraftStatusDeltaLine(
            "Pending assumptions",
            std::to_string(delta.CanonicalStatus.PendingAssumptionCount),
            std::to_string(delta.DraftStatus.PendingAssumptionCount));
        DrawDraftStatusDeltaLine(
            "Deferred semantics",
            std::to_string(delta.CanonicalStatus.DeferredCount),
            std::to_string(delta.DraftStatus.DeferredCount));
        DrawDraftStatusDeltaLine(
            "Model class",
            delta.CanonicalStatus.Classification.ModelClass,
            delta.DraftStatus.Classification.ModelClass);
        DrawDraftStatusDeltaLine(
            "Character",
            delta.CanonicalStatus.Classification.Character,
            delta.DraftStatus.Classification.Character);

        ImGui::SeparatorText("Draft Interpretation");
        if (delta.CanonicalInferredKind.has_value() || delta.DraftInferredKind.has_value()) {
            DrawDraftStatusDeltaLine(
                "Inferred role",
                delta.CanonicalInferredKind.has_value() ? ModelV2::ToString(*delta.CanonicalInferredKind) : "-",
                delta.DraftInferredKind.has_value() ? ModelV2::ToString(*delta.DraftInferredKind) : "-");
        }
        if (delta.CanonicalRelationClass.has_value() || delta.DraftRelationClass.has_value()) {
            DrawDraftStatusDeltaLine(
                "Relation class",
                delta.CanonicalRelationClass.has_value() ? ModelV2::ToString(*delta.CanonicalRelationClass) : "-",
                delta.DraftRelationClass.has_value() ? ModelV2::ToString(*delta.DraftRelationClass) : "-");
        }
        DrawDraftStatusDeltaLine(
            "Conflict state",
            delta.bCanonicalConflict ? "warning" : "clear",
            delta.bDraftConflict ? "warning" : "clear");
        if (!delta.DraftInterpretation.empty()) {
            if (delta.CanonicalInterpretation != delta.DraftInterpretation && !delta.CanonicalInterpretation.empty()) {
                ImGui::TextDisabled("Canonical: %s", delta.CanonicalInterpretation.c_str());
            }
            ImGui::TextWrapped("Draft: %s", delta.DraftInterpretation.c_str());
        }

        if (!DraftDeltaHasChanges(delta)) {
            ImGui::TextDisabled("No semantic delta from canonical state.");
            return;
        }

        DrawReferencedSymbolDeltaSection("Reference Changes", delta.ReferencedSymbolChanges);
        DrawNavigationDeltaSection("Dependency Changes", delta.DependencyChanges, navigate);
        DrawNavigationDeltaSection("Ambient Dependency Changes", delta.AmbientDependencyChanges, navigate);
        DrawNavigationDeltaSection("Reverse Dependency Changes", delta.UsedByChanges, navigate);
        DrawNavigationDeltaSection("Assumption Link Changes", delta.AssumptionLinkChanges, navigate);
        DrawNavigationDeltaSection("Override / Specialization Changes", delta.OverrideChanges, navigate);
        DrawAssumptionDeltaSection("Assumption Changes", delta.AssumptionChanges, navigate);
        DrawDiagnosticDeltaSection("Diagnostic Changes", delta.DiagnosticChanges);
    }

    template<typename FNavigate>
    auto DrawDefinitionInspector(Typesetting::FTypesettingService &typesettingService,
                                 const ModelV2::FModelV2 &model,
                                 const ModelV2::FDefinitionV2 &definition,
                                 const ModelV2::FSemanticObjectOverviewV2 &objectOverview,
                                 const ModelV2::FDefinitionSemanticReportV2 *report,
                                 const ModelV2::FDefinitionDraftPreviewV2 *draftPreview,
                                 const FModelInspectorTheme &theme,
                                 const FNavigate &navigate) -> void {
        const auto title = definition.DisplayName.empty() ? definition.DefinitionId : definition.DisplayName;
        Typesetting::ImGuiTypesetText(typesettingService, title, theme.SectionTextOptions);
        ImGui::TextDisabled("%s | %s", definition.DefinitionId.c_str(), objectOverview.KindLabel.c_str());
        Typesetting::ImGuiTypesetMath(
            typesettingService,
            ModelV2::RenderDialectDefinitionV2(definition, &model),
            theme.DetailMathOptions);

        ImGui::SeparatorText("Identity");
        ImGui::BulletText("Origin: %s", objectOverview.OriginLabel.c_str());
        if (!objectOverview.OriginDetail.empty()) ImGui::BulletText("Origin detail: %s", objectOverview.OriginDetail.c_str());
        ImGui::BulletText("Status: %s", objectOverview.StatusLabel.c_str());
        ImGui::BulletText("Kind: %s", objectOverview.KindLabel.c_str());
        if (report != nullptr && report->InferredKind.has_value()) {
            ImGui::BulletText("Inferred role: %s", ModelV2::ToString(*report->InferredKind));
            if (report->bRoleMismatchesDeclared) {
                ImGui::TextColored(ImVec4(0.86f, 0.47f, 0.26f, 1.0f), "Declared role disagrees with inferred role.");
            }
        }
        if (definition.DeclaredType.has_value()) {
            Typesetting::ImGuiTypesetLabelValue(
                typesettingService,
                "Declared Type",
                MakeMathRequest(ModelV2::RenderDialectTypeExprV2(*definition.DeclaredType, &model), theme.BaseFontSize),
                theme.SecondaryTextOptions,
                theme.DetailMathOptions);
        }
        if (!objectOverview.Description.empty()) {
            ImGui::TextWrapped("%s", objectOverview.Description.c_str());
        }
        if (report != nullptr && !report->NormalizedInterpretation.empty()) {
            ImGui::TextWrapped("%s", report->NormalizedInterpretation.c_str());
        }

        if (draftPreview != nullptr && draftPreview->bParseOk) {
            ImGui::SeparatorText("Draft Preview");
            Typesetting::ImGuiTypesetMath(typesettingService, draftPreview->DraftNotation, theme.DetailMathOptions);
            Typesetting::ImGuiTypesetLabelValue(
                typesettingService,
                "Normalized",
                MakeMathRequest(draftPreview->NormalizedProjection, theme.BaseFontSize),
                theme.SecondaryTextOptions,
                theme.DetailMathOptions);
        }

        DrawSemanticLinkSection("Origin / Source", objectOverview.SourceLinks, navigate);
        DrawSemanticLinkSection("Depends On", objectOverview.DependsOn, navigate);
        DrawSemanticLinkSection("Ambient Dependencies", objectOverview.AmbientDependencies, navigate);
        DrawSemanticLinkSection("Used By", objectOverview.UsedBy, navigate);
        DrawSemanticLinkSection("Related Assumptions", objectOverview.RelatedAssumptions, navigate);
        DrawSemanticLinkSection("Local Override / Ambient Link", objectOverview.LocalOverrides, navigate);
        DrawDiagnosticNavigationSection(objectOverview.Diagnostics, navigate);
    }

    template<typename FNavigate>
    auto DrawRelationInspector(Typesetting::FTypesettingService &typesettingService,
                               const ModelV2::FModelV2 &model,
                               const ModelV2::FRelationV2 &relation,
                               const ModelV2::FSemanticObjectOverviewV2 &objectOverview,
                               const ModelV2::FRelationSemanticReportV2 *report,
                               const ModelV2::FRelationDraftPreviewV2 *draftPreview,
                               const FModelInspectorTheme &theme,
                               const FNavigate &navigate) -> void {
        const auto title = relation.Name.empty() ? relation.RelationId : relation.Name;
        Typesetting::ImGuiTypesetText(typesettingService, title, theme.SectionTextOptions);
        ImGui::TextDisabled("%s | %s", relation.RelationId.c_str(), objectOverview.KindLabel.c_str());
        Typesetting::ImGuiTypesetMath(
            typesettingService,
            ModelV2::RenderDialectRelationV2(relation, &model),
            theme.DetailMathOptions);

        ImGui::SeparatorText("Identity");
        ImGui::BulletText("Origin: %s", objectOverview.OriginLabel.c_str());
        ImGui::BulletText("Status: %s", objectOverview.StatusLabel.c_str());
        if (report != nullptr) {
            ImGui::BulletText("Inferred class: %s", ModelV2::ToString(report->InferredClass));
            if (!report->NormalizedInterpretation.empty()) {
                ImGui::TextWrapped("%s", report->NormalizedInterpretation.c_str());
            }
        }
        if (!objectOverview.Description.empty()) {
            ImGui::TextWrapped("%s", objectOverview.Description.c_str());
        }

        if (draftPreview != nullptr && draftPreview->bParseOk) {
            ImGui::SeparatorText("Draft Preview");
            Typesetting::ImGuiTypesetMath(typesettingService, draftPreview->DraftNotation, theme.DetailMathOptions);
            Typesetting::ImGuiTypesetLabelValue(
                typesettingService,
                "Normalized",
                MakeMathRequest(draftPreview->NormalizedProjection, theme.BaseFontSize),
                theme.SecondaryTextOptions,
                theme.DetailMathOptions);
        }

        DrawSemanticLinkSection("Depends On", objectOverview.DependsOn, navigate);
        DrawSemanticLinkSection("Ambient Dependencies", objectOverview.AmbientDependencies, navigate);
        DrawSemanticLinkSection("Related Assumptions", objectOverview.RelatedAssumptions, navigate);
        DrawDiagnosticNavigationSection(objectOverview.Diagnostics, navigate);
    }

    template<typename FNavigate>
    auto DrawVocabularyInspector(Typesetting::FTypesettingService &typesettingService,
                                 const ModelV2::FResolvedBaseVocabularyEntryV2 &entry,
                                 const ModelV2::FSemanticObjectOverviewV2 &objectOverview,
                                 const FModelInspectorTheme &theme,
                                 const FNavigate &navigate) -> void {
        const auto title = entry.Entry.DisplayName.empty() ? entry.Entry.EntryId : entry.Entry.DisplayName;
        Typesetting::ImGuiTypesetText(typesettingService, title, theme.SectionTextOptions);
        ImGui::TextDisabled("%s | %s", entry.Entry.EntryId.c_str(), objectOverview.KindLabel.c_str());
        Typesetting::ImGuiTypesetMath(
            typesettingService,
            ModelV2::RenderBaseVocabularyEntryLatexV2(entry.Entry),
            theme.DetailMathOptions);

        ImGui::SeparatorText("Identity");
        ImGui::BulletText("Origin: %s", objectOverview.OriginLabel.c_str());
        ImGui::BulletText("Status: %s", objectOverview.StatusLabel.c_str());
        if (!objectOverview.OriginDetail.empty()) ImGui::BulletText("Preset: %s", objectOverview.OriginDetail.c_str());
        if (entry.Entry.DefinitionKind.has_value()) {
            ImGui::BulletText("Definition-like role: %s", ModelV2::ToString(*entry.Entry.DefinitionKind));
        }
        if (!entry.Entry.SemanticRoleSummary.empty()) {
            ImGui::TextWrapped("%s", entry.Entry.SemanticRoleSummary.c_str());
        }
        if (!entry.Entry.Description.empty()) {
            ImGui::TextWrapped("%s", entry.Entry.Description.c_str());
        }

        DrawSemanticLinkSection("Used By", objectOverview.UsedBy, navigate);
        DrawSemanticLinkSection("Related Assumptions", objectOverview.RelatedAssumptions, navigate);
        DrawSemanticLinkSection("Local Override / Specialization", objectOverview.LocalOverrides, navigate);
        DrawDiagnosticNavigationSection(objectOverview.Diagnostics, navigate);
    }

    template<typename FNavigate>
    auto DrawAssumptionInspector(Typesetting::FTypesettingService &typesettingService,
                                 const ModelV2::FModelV2 &model,
                                 const ModelV2::FSemanticAssumptionV2 &assumption,
                                 const ModelV2::FSemanticObjectOverviewV2 &objectOverview,
                                 const ModelV2::FAssumptionMaterializationPreviewV2 &materializationPreview,
                                 const FModelInspectorTheme &theme,
                                 const FNavigate &navigate) -> void {
        const auto title = assumption.TargetSymbol.empty()
            ? (assumption.Category.empty() ? assumption.AssumptionId : assumption.Category)
            : assumption.TargetSymbol;
        Typesetting::ImGuiTypesetText(typesettingService, title, theme.SectionTextOptions);
        ImGui::TextDisabled("%s | %s", assumption.AssumptionId.c_str(), objectOverview.KindLabel.c_str());

        ImGui::SeparatorText("Identity");
        ImGui::BulletText("Origin: %s", objectOverview.OriginLabel.c_str());
        ImGui::BulletText("Status: %s", objectOverview.StatusLabel.c_str());
        ImGui::BulletText("Lifecycle: %s", materializationPreview.LifecycleLabel.c_str());
        if (!assumption.Category.empty()) ImGui::BulletText("Category: %s", assumption.Category.c_str());
        if (assumption.DeclaredKind.has_value() || assumption.InferredKind.has_value()) {
            ImGui::BulletText(
                "Declared: %s | Inferred: %s",
                assumption.DeclaredKind.has_value() ? ModelV2::ToString(*assumption.DeclaredKind) : "-",
                assumption.InferredKind.has_value() ? ModelV2::ToString(*assumption.InferredKind) : "-");
        }
        if (assumption.bMatchesDeclaredRole) {
            ImGui::TextDisabled("This assumption agrees with the explicit local meaning.");
        } else if (assumption.bMismatchesDeclaredRole) {
            ImGui::TextColored(ImVec4(0.86f, 0.47f, 0.26f, 1.0f), "This assumption conflicts with the explicit local meaning.");
        }
        if (!assumption.Detail.empty()) {
            ImGui::TextWrapped("%s", assumption.Detail.c_str());
        }
        if (!assumption.ProposedArgumentDefinitionIds.empty()) {
            Slab::Str dependencySummary;
            for (std::size_t i = 0; i < assumption.ProposedArgumentDefinitionIds.size(); ++i) {
                if (i != 0) dependencySummary += ", ";
                dependencySummary += assumption.ProposedArgumentDefinitionIds[i];
            }
            ImGui::TextDisabled("Proposed dependency: %s", dependencySummary.c_str());
        }
        if (!materializationPreview.OutcomeLabel.empty()) {
            ImGui::TextWrapped("%s", materializationPreview.OutcomeLabel.c_str());
        }
        if (assumption.ProposedType.has_value()) {
            Typesetting::ImGuiTypesetLabelValue(
                typesettingService,
                "Proposed Type",
                MakeMathRequest(ModelV2::RenderDialectTypeExprV2(*assumption.ProposedType, &model), theme.BaseFontSize),
                theme.SecondaryTextOptions,
                theme.DetailMathOptions);
        }

        if (materializationPreview.bAlreadyMaterialized) {
            ImGui::SeparatorText("Materialized Local Artifact");
            if (materializationPreview.MaterializedObjectRef.has_value() &&
                ImGui::SmallButton(materializationPreview.MaterializedObjectRef->ObjectId.c_str())) {
                navigate(*materializationPreview.MaterializedObjectRef);
            }
        } else if (materializationPreview.ProposedDefinition.has_value()) {
            ImGui::SeparatorText("Materialization Preview");
            ImGui::BulletText("Suggested definition id: %s", materializationPreview.SuggestedDefinitionId.c_str());
            ImGui::BulletText("Proposed kind: %s", ModelV2::ToString(materializationPreview.ProposedDefinition->Kind));
            Typesetting::ImGuiTypesetMath(
                typesettingService,
                materializationPreview.ProposedNotation,
                theme.DetailMathOptions);
            DrawSemanticLinkSection("Proposed Dependencies", materializationPreview.ProposedDependencies, navigate);
        }

        DrawSemanticLinkSection("Source", objectOverview.SourceLinks, navigate);
        DrawSemanticLinkSection("Affected Object", objectOverview.TargetLinks, navigate);
        DrawSemanticLinkSection("Related Ambient / Override Links", objectOverview.LocalOverrides, navigate);
        DrawDiagnosticNavigationSection(objectOverview.Diagnostics, navigate);
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
    AddTooltipForLastItem("Switch between seeded reference models. Changing the seed resets selection and editor draft state for this Model tab session.");

    auto &model = ModelDemoCatalog[SelectedModelIndex];
    if (bModelChanged) {
        SelectedModelSemanticObject = {};
        bSelectedModelSemanticObjectUsesDraftPreview = false;
        ModelPendingScrollTarget = {};
        SelectedModelVocabularyEntryId.clear();
        SelectedModelDefinitionId.clear();
        SelectedModelRelationId.clear();
        SelectedModelAssumptionId.clear();
        bSelectedModelDetailIsRelation = false;
        ModelEditorBuffersByKey.clear();
        ModelEditorStatus.clear();
        bModelHasLastChangeRecord = false;
        InvalidateModelWorkspaceViewState();
    }

    const auto &vocabularyPresets = GetBaseVocabularyPresetCatalogV2();
    if ((model.BaseVocabulary.ActivePresetId.empty() ||
         FindBaseVocabularyPresetByIdV2(model.BaseVocabulary.ActivePresetId) == nullptr) &&
        !vocabularyPresets.empty()) {
        model.BaseVocabulary.ActivePresetId = vocabularyPresets.front().PresetId;
    }

    bool bVocabularyChanged = false;
    const auto *activeVocabularyPreset = FindBaseVocabularyPresetByIdV2(model.BaseVocabulary.ActivePresetId);
    if (activeVocabularyPreset == nullptr && !vocabularyPresets.empty()) {
        activeVocabularyPreset = &vocabularyPresets.front();
        model.BaseVocabulary.ActivePresetId = activeVocabularyPreset->PresetId;
    }

    if (ImGui::BeginCombo(
            "Base Vocabulary",
            activeVocabularyPreset != nullptr ? activeVocabularyPreset->Name.c_str() : "<none>")) {
        for (const auto &preset : vocabularyPresets) {
            const bool bSelected = preset.PresetId == model.BaseVocabulary.ActivePresetId;
            if (ImGui::Selectable(preset.Name.c_str(), bSelected)) {
                model.BaseVocabulary.ActivePresetId = preset.PresetId;
                bVocabularyChanged = true;
            }
            if (bSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    AddTooltipForLastItem("Select the active ambient Base Vocabulary / Semantic Environment for this model. Local Definitions remain explicit and take precedence.");
    if (bVocabularyChanged) {
        ModelPendingScrollTarget = {};
        SelectedModelSemanticObject = {};
        bSelectedModelSemanticObjectUsesDraftPreview = false;
        SelectedModelVocabularyEntryId.clear();
        SelectedModelAssumptionId.clear();
        ModelEditorBuffersByKey.clear();
        ModelEditorStatus = "[Ok] Switched base vocabulary to '" + model.BaseVocabulary.ActivePresetId + "'.";
        InvalidateModelWorkspaceViewState();
    }

    const auto overview = BuildModelSemanticOverviewV2(model);
    const auto &semanticReport = overview.Report;
    const auto &validation = overview.Validation;
    const auto &resolvedVocabularyEntries = overview.VocabularyEntries;

    if (SelectedModelVocabularyEntryId.empty() && !resolvedVocabularyEntries.empty()) {
        SelectedModelVocabularyEntryId = resolvedVocabularyEntries.front().Entry.EntryId;
    }
    if (!SelectedModelVocabularyEntryId.empty() &&
        FindResolvedVocabularyEntry(resolvedVocabularyEntries, SelectedModelVocabularyEntryId) == nullptr &&
        !resolvedVocabularyEntries.empty()) {
        SelectedModelVocabularyEntryId = resolvedVocabularyEntries.front().Entry.EntryId;
    }

    if (SelectedModelDefinitionId.empty() && !model.Definitions.empty()) {
        SelectedModelDefinitionId = model.Definitions.front().DefinitionId;
    }
    if (SelectedModelRelationId.empty() && !model.Relations.empty()) {
        SelectedModelRelationId = model.Relations.front().RelationId;
    }
    if (!SelectedModelDefinitionId.empty() &&
        FindDefinitionByIdV2(model, SelectedModelDefinitionId) == nullptr &&
        !model.Definitions.empty()) {
        SelectedModelDefinitionId = model.Definitions.front().DefinitionId;
    }
    if (!SelectedModelRelationId.empty() &&
        FindRelationByIdV2(model, SelectedModelRelationId) == nullptr &&
        !model.Relations.empty()) {
        SelectedModelRelationId = model.Relations.front().RelationId;
    }

    const auto editorTargetKind = bSelectedModelDetailIsRelation ? EModelObjectKindV2::Relation : EModelObjectKindV2::Definition;
    const auto editorTargetId = bSelectedModelDetailIsRelation ? SelectedModelRelationId : SelectedModelDefinitionId;
    auto *activeEditorBuffer = EnsureEditorBuffer(ModelEditorBuffersByKey, model, editorTargetKind, editorTargetId);
    const auto *activeDraftOverview = GetEditorBufferPreviewOverview(activeEditorBuffer);

    auto selectSemanticObject = [&](const FSemanticObjectRefV2 &ref,
                                    const bool bRequestScroll = false,
                                    const bool bUseDraftPreview = false) {
        if (!ref.IsValid()) return;
        SelectedModelSemanticObject = ref;
        bSelectedModelSemanticObjectUsesDraftPreview = bUseDraftPreview;
        if (bRequestScroll) ModelPendingScrollTarget = ref;

        switch (ref.Kind) {
            case ESemanticObjectKindV2::Definition:
                if (!bUseDraftPreview && FindDefinitionByIdV2(model, ref.ObjectId) != nullptr) {
                    SelectedModelDefinitionId = ref.ObjectId;
                    bSelectedModelDetailIsRelation = false;
                }
                break;
            case ESemanticObjectKindV2::Relation:
                if (!bUseDraftPreview && FindRelationByIdV2(model, ref.ObjectId) != nullptr) {
                    SelectedModelRelationId = ref.ObjectId;
                    bSelectedModelDetailIsRelation = true;
                }
                break;
            case ESemanticObjectKindV2::VocabularyEntry:
                SelectedModelVocabularyEntryId = ref.ObjectId;
                break;
            case ESemanticObjectKindV2::Assumption:
                SelectedModelAssumptionId = ref.ObjectId;
                break;
        }
        InvalidateModelWorkspaceViewState();
    };

    auto navigateToSemanticObject = [&](const FSemanticObjectRefV2 &ref) {
        selectSemanticObject(ref, true, false);
    };

    auto navigateToDraftSemanticObject = [&](const FSemanticObjectRefV2 &ref) {
        selectSemanticObject(ref, true, true);
    };

    const auto *selectionOverview = &overview;
    if (SelectedModelSemanticObject.IsValid()) {
        const bool bSelectionIsActiveEditorTarget =
            (editorTargetKind == EModelObjectKindV2::Definition &&
             SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Definition &&
             SelectedModelSemanticObject.ObjectId == editorTargetId) ||
            (editorTargetKind == EModelObjectKindV2::Relation &&
             SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Relation &&
             SelectedModelSemanticObject.ObjectId == editorTargetId);

        if (activeDraftOverview != nullptr &&
            (bSelectedModelSemanticObjectUsesDraftPreview || bSelectionIsActiveEditorTarget) &&
            activeDraftOverview->FindObject(SelectedModelSemanticObject) != nullptr) {
            selectionOverview = activeDraftOverview;
            bSelectedModelSemanticObjectUsesDraftPreview = bSelectedModelSemanticObjectUsesDraftPreview || bSelectionIsActiveEditorTarget;
        } else if (overview.FindObject(SelectedModelSemanticObject) != nullptr) {
            bSelectedModelSemanticObjectUsesDraftPreview = false;
        } else if (activeDraftOverview != nullptr && activeDraftOverview->FindObject(SelectedModelSemanticObject) != nullptr) {
            bSelectedModelSemanticObjectUsesDraftPreview = true;
            selectionOverview = activeDraftOverview;
        } else {
            SelectedModelSemanticObject = {};
            bSelectedModelSemanticObjectUsesDraftPreview = false;
        }
    }
    if (!SelectedModelSemanticObject.IsValid()) {
        if (!SelectedModelDefinitionId.empty() && FindDefinitionByIdV2(model, SelectedModelDefinitionId) != nullptr) {
            selectSemanticObject(MakeDefinitionObjectRefV2(SelectedModelDefinitionId), false, false);
        } else if (!SelectedModelRelationId.empty() && FindRelationByIdV2(model, SelectedModelRelationId) != nullptr) {
            selectSemanticObject(MakeRelationObjectRefV2(SelectedModelRelationId), false, false);
        } else if (!SelectedModelVocabularyEntryId.empty()) {
            selectSemanticObject(MakeVocabularyObjectRefV2(SelectedModelVocabularyEntryId), false, false);
        } else if (!semanticReport.Assumptions.empty()) {
            selectSemanticObject(MakeAssumptionObjectRefV2(semanticReport.Assumptions.front().AssumptionId), false, false);
        }
    }
    if (!SelectedModelSemanticObject.IsValid() && !resolvedVocabularyEntries.empty()) {
        selectSemanticObject(MakeVocabularyObjectRefV2(resolvedVocabularyEntries.front().Entry.EntryId), false, false);
    }
    if (SelectedModelSemanticObject.IsValid() &&
        bSelectedModelSemanticObjectUsesDraftPreview &&
        activeDraftOverview != nullptr &&
        activeDraftOverview->FindObject(SelectedModelSemanticObject) != nullptr) {
        selectionOverview = activeDraftOverview;
    } else {
        selectionOverview = &overview;
        bSelectedModelSemanticObjectUsesDraftPreview = false;
    }

    const auto selectionContext = BuildSemanticSelectionContextV2(*selectionOverview, SelectedModelSemanticObject);
    const auto draftPreviewDefinitions = CollectDraftProposedDefinitions(model, activeEditorBuffer, activeDraftOverview);
    const auto *draftPreviewAssumptions = GetEditorBufferPreviewAssumptions(activeEditorBuffer);
    SyncModelSemanticGraphWindow(model, *selectionOverview, bSelectedModelSemanticObjectUsesDraftPreview);

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

    DrawSemanticSummaryBadge("Parse", ToString(overview.Status.ParseHealth), HealthTint(overview.Status.ParseHealth));
    ImGui::SameLine();
    DrawSemanticSummaryBadge("Semantic", ToString(overview.Status.SemanticHealth), HealthTint(overview.Status.SemanticHealth));
    ImGui::SameLine();
    ImGui::TextDisabled("Unresolved: %zu", overview.Status.UnresolvedSymbolCount);
    ImGui::SameLine();
    ImGui::TextDisabled("Pending assumptions: %zu", overview.Status.PendingAssumptionCount);

    ImGui::TextDisabled(
        "Inferred class: %s | Character: %s | Base Vocabulary: %s",
        overview.Status.Classification.ModelClass.c_str(),
        overview.Status.Classification.Character.c_str(),
        overview.Status.ActiveBaseVocabularyName.c_str());
    AddTooltipForLastItem("Compact model-level semantic summary inferred from the active relations, local definitions, assumptions, and active ambient vocabulary.");

    ImGui::TextDisabled(
        "Overrides: %zu | Specializations: %zu | Validation errors: %zu",
        overview.Status.OverrideCount,
        overview.Status.SpecializationCount,
        validation.ErrorCount());
    ImGui::TextDisabled(
        "Accepted assumptions: %zu | Dismissed: %zu | Deferred: %zu",
        overview.Status.AcceptedAssumptionCount,
        overview.Status.DismissedAssumptionCount,
        overview.Status.DeferredCount);

    DrawSummaryLinkStrip("State", overview.Status.CanonicalStateVariables, navigateToSemanticObject);
    DrawSummaryLinkStrip("Parameters", overview.Status.Parameters, navigateToSemanticObject);
    DrawSummaryLinkStrip("Fields", overview.Status.Fields, navigateToSemanticObject);
    DrawSummaryLinkStrip("Operators", overview.Status.Operators, navigateToSemanticObject);
    DrawSummaryLinkStrip("Observables", overview.Status.Observables, navigateToSemanticObject);

    ImGui::SeparatorText("Semantic Graph");
    if (ImGui::Button("Open in Plots")) {
        FocusModelSemanticGraphWindow();
    }
    AddTooltipForLastItem(
        "Open the read-only semantic knowledge graph as a Plot2D V2 surface. "
        "Click nodes to reuse the current model selection; use [ and ] to change neighborhood radius, "
        "L to toggle labels, and F to fit the graph.");
    if (SelectedModelSemanticObject.IsValid()) {
        ImGui::SameLine();
        ImGui::TextDisabled("Centered on: %s", SelectedModelSemanticObject.ObjectId.c_str());
    }

    const auto odeDescriptor = BuildODERealizationDescriptorV2(model, &overview);
    const auto realizationBadgeTint = odeDescriptor.IsReady()
        ? ImVec4(0.33f, 0.67f, 0.45f, 1.0f)
        : ImVec4(0.82f, 0.28f, 0.28f, 1.0f);
    ImGui::SeparatorText("ODE Realization");
    DrawSemanticSummaryBadge("Readiness", ToString(odeDescriptor.Readiness), realizationBadgeTint);
    ImGui::SameLine();
    ImGui::TextDisabled("Strategy: %s", ToString(odeDescriptor.Strategy));
    AddTooltipForLastItem(
        "Derived from canonical model semantics only. This slice stays conservative: it selects explicit "
        "first-order state equations, requires one explicit model-level initial state, and does not commit "
        "to a solver/runtime policy.");

    if (odeDescriptor.TimeCoordinate.has_value()) {
        ImGui::TextDisabled("Time:");
        ImGui::SameLine();
        if (ImGui::SmallButton(odeDescriptor.TimeCoordinate->DisplayLabel.c_str())) {
            navigateToSemanticObject(MakeDefinitionObjectRefV2(odeDescriptor.TimeCoordinate->DefinitionId));
        }
        AddTooltipForLastItem(odeDescriptor.TimeCoordinate->CanonicalNotation);
    } else {
        ImGui::TextDisabled("Time: unresolved for ODE descent");
    }

    if (odeDescriptor.InitialTimeNotation.has_value()) {
        ImGui::TextDisabled("Initial time: %s", odeDescriptor.InitialTimeNotation->c_str());
    } else {
        ImGui::TextDisabled("Initial time: unresolved for ODE descent");
    }

    if (!odeDescriptor.SelectedRelations.empty()) {
        ImGui::TextDisabled("Selected relations:");
        ImGui::SameLine();
        for (std::size_t i = 0; i < odeDescriptor.SelectedRelations.size(); ++i) {
            const auto &selectedRelation = odeDescriptor.SelectedRelations[i];
            ImGui::PushID(static_cast<int>(i));
            const auto buttonLabel = selectedRelation.StateDisplayLabel + " <- " + selectedRelation.DisplayLabel;
            if (ImGui::SmallButton(buttonLabel.c_str())) {
                navigateToSemanticObject(MakeRelationObjectRefV2(selectedRelation.RelationId));
            }
            AddTooltipForLastItem(
                selectedRelation.CanonicalNotation + "\n" +
                selectedRelation.DerivativeNotation + " = " + selectedRelation.ExplicitExpressionNotation);
            ImGui::PopID();
            if (i + 1 < odeDescriptor.SelectedRelations.size()) ImGui::SameLine();
        }
    } else {
        ImGui::TextDisabled("Selected relations: none");
    }

    if (!odeDescriptor.InitialConditions.empty()) {
        ImGui::TextDisabled("Initial state:");
        ImGui::SameLine();
        for (std::size_t i = 0; i < odeDescriptor.InitialConditions.size(); ++i) {
            const auto &initialCondition = odeDescriptor.InitialConditions[i];
            ImGui::PushID(static_cast<int>(i + 1000));
            const auto buttonLabel = initialCondition.StateDisplayLabel + " <- " + initialCondition.ValueNotation;
            if (ImGui::SmallButton(buttonLabel.c_str())) {
                navigateToSemanticObject(MakeDefinitionObjectRefV2(initialCondition.StateDefinitionId));
            }
            AddTooltipForLastItem(
                "Explicit model-level initial condition for ODE descent.\n" +
                initialCondition.StateDisplayLabel + "(t_0) = " + initialCondition.ValueNotation);
            ImGui::PopID();
            if (i + 1 < odeDescriptor.InitialConditions.size()) ImGui::SameLine();
        }
    } else {
        ImGui::TextDisabled("Initial state: none");
    }

    if (!odeDescriptor.Diagnostics.empty()) {
        ImGui::TextDisabled("Diagnostics:");
        for (std::size_t i = 0; i < odeDescriptor.Diagnostics.size(); ++i) {
            const auto &diagnostic = odeDescriptor.Diagnostics[i];
            ImGui::PushID(static_cast<int>(i));
            ImGui::PushStyleColor(
                ImGuiCol_Text,
                diagnostic.Severity == EValidationSeverityV2::Error
                    ? ImVec4(0.82f, 0.28f, 0.28f, 1.0f)
                    : ImVec4(0.86f, 0.55f, 0.22f, 1.0f));
            ImGui::BulletText("%s", diagnostic.Message.c_str());
            ImGui::PopStyleColor();

            if (diagnostic.Source.has_value()) {
                ImGui::SameLine();
                if (ImGui::SmallButton("Jump")) {
                    navigateToSemanticObject(*diagnostic.Source);
                }
            }
            ImGui::PopID();
        }
    }

    if (!overview.Status.Classification.Traits.empty()) {
        Slab::Str traits;
        for (std::size_t i = 0; i < overview.Status.Classification.Traits.size(); ++i) {
            if (i != 0) traits += ", ";
            traits += overview.Status.Classification.Traits[i];
        }
        ImGui::TextDisabled("Traits: %s", traits.c_str());
    }

    if (bModelHasLastChangeRecord) {
        ImGui::TextDisabled(
            "Last change: %s %s | %s",
            ToString(ModelLastChangeRecord.Origin),
            ToString(ModelLastChangeRecord.ObjectKind),
            ModelLastChangeRecord.ObjectId.c_str());
    }
    if (!ModelEditorStatus.empty()) {
        ImGui::TextWrapped("%s", ModelEditorStatus.c_str());
    }

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
    bool bUiStateChangedThisFrame = false;

    ImGui::TextDisabled("Selection drives the inspector. The editor follows the last selected definition or relation.");

    auto consumePendingScroll = [&](const FSemanticObjectRefV2 &ref) {
        if (!ModelPendingScrollTarget.IsValid()) return;
        if (!AreSemanticObjectRefsEqualV2(ModelPendingScrollTarget, ref)) return;
        ImGui::SetScrollHereY(0.35f);
        ModelPendingScrollTarget = {};
    };

    if (topPaneHeight > 0.0f) {
        ImGui::BeginChild("ModelTopPane", ImVec2(0.0f, topPaneHeight), false, ImGuiWindowFlags_NoScrollbar);

        if (ImGui::BeginTable(
                "ModelCatalogLayout",
                2,
                ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Catalog", ImGuiTableColumnFlags_WidthFixed, listColumnWidth);
            ImGui::TableSetupColumn("Inspector", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextColumn();

            const float splitterThickness = 12.0f;
            const float catalogRailHeight = std::max(ImGui::GetContentRegionAvail().y, 0.0f);
            const float minVocabularyPanelHeight = std::max(baseFontSize * 4.2f, 92.0f);
            const float minCatalogBodyHeight = 120.0f;
            const float maxVocabularyPanelHeight = std::max(
                minVocabularyPanelHeight,
                catalogRailHeight - splitterThickness - minCatalogBodyHeight);
            float vocabularyPanelHeight = std::clamp(
                ModelCatalogVocabularyHeight,
                minVocabularyPanelHeight,
                maxVocabularyPanelHeight);

            ImGui::BeginChild("ModelCatalogRail", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_NoScrollbar);
            ImGui::BeginChild("ModelVocabularySection", ImVec2(0.0f, vocabularyPanelHeight), true);
            ImGui::SeparatorText("Base Vocabulary");
            ImGui::TextDisabled("Ambient / Readonly");
            if (activeVocabularyPreset != nullptr) {
                ImGui::TextDisabled("%s (%s)", activeVocabularyPreset->Name.c_str(), activeVocabularyPreset->PresetId.c_str());
            }
            ImGui::Dummy(ImVec2(0.0f, baseFontSize * 0.55f));

            for (const auto &entry : resolvedVocabularyEntries) {
                const auto ref = MakeVocabularyObjectRefV2(entry.Entry.EntryId);
                const auto highlightState = ResolveHighlightState(selectionContext, *selectionOverview, ref);
                ImGui::PushID(entry.Entry.EntryId.c_str());
                const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                    typesettingService,
                    "##ModelVocabularyEntryRow",
                    MakeMathRequest(RenderBaseVocabularyEntryLatexV2(entry.Entry), baseFontSize * 0.98f),
                    MakeVocabularyRowOptions(highlightState, baseFontSize));
                AddTooltipForLastItem("Select this ambient entry to inspect provenance, reverse dependencies, and local override/specialization links.");
                if (rowResult.bPressed) {
                    selectSemanticObject(ref);
                }
                consumePendingScroll(ref);
                ImGui::PopID();
            }
            ImGui::EndChild();

            ImGui::InvisibleButton("##ModelVocabularySplitter", ImVec2(-FLT_MIN, splitterThickness));
            const bool bVocabularySplitterHovered = ImGui::IsItemHovered();
            const bool bVocabularySplitterActive = ImGui::IsItemActive();
            if (bVocabularySplitterHovered || bVocabularySplitterActive) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            if (bVocabularySplitterActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
                vocabularyPanelHeight = std::clamp(
                    vocabularyPanelHeight + ImGui::GetIO().MouseDelta.y,
                    minVocabularyPanelHeight,
                    maxVocabularyPanelHeight);
                ModelCatalogVocabularyHeight = vocabularyPanelHeight;
            } else {
                ModelCatalogVocabularyHeight = vocabularyPanelHeight;
            }

            const auto vocabularySplitterRectMin = ImGui::GetItemRectMin();
            const auto vocabularySplitterRectMax = ImGui::GetItemRectMax();
            const float vocabularySplitterCenterY = 0.5f * (vocabularySplitterRectMin.y + vocabularySplitterRectMax.y);
            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(vocabularySplitterRectMin.x + 2.0f, vocabularySplitterCenterY),
                ImVec2(vocabularySplitterRectMax.x - 2.0f, vocabularySplitterCenterY),
                bVocabularySplitterActive ? IM_COL32(246, 202, 116, 210) : IM_COL32(96, 108, 124, 192),
                bVocabularySplitterActive ? 2.0f : 1.0f);

            const float catalogBodyHeight = std::max(ImGui::GetContentRegionAvail().y, 0.0f);
            const float minCatalogPaneHeight = std::max(
                48.0f,
                std::min(
                    std::max(baseFontSize * 4.0f, 96.0f),
                    std::max(48.0f, (catalogBodyHeight - splitterThickness) * 0.4f)));
            const float maxDefinitionsHeight =
                std::max(minCatalogPaneHeight, catalogBodyHeight - minCatalogPaneHeight - splitterThickness);
            float definitionsPanelHeight =
                std::clamp(ModelCatalogDefinitionsHeight, minCatalogPaneHeight, maxDefinitionsHeight);

            ImGui::BeginChild("ModelDefinitionsList", ImVec2(0.0f, definitionsPanelHeight), true);
            ImGui::SeparatorText("Definitions");
            for (const auto &definition : model.Definitions) {
                const auto ref = MakeDefinitionObjectRefV2(definition.DefinitionId);
                const auto highlightState = ResolveHighlightState(selectionContext, *selectionOverview, ref);
                ImGui::PushID(definition.DefinitionId.c_str());
                const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                    typesettingService,
                    "##ModelDefinitionRow",
                    MakeMathRequest(RenderDialectDefinitionV2(definition, &model), baseFontSize * 1.02f),
                    MakeDefinitionRowOptions(highlightState, baseFontSize));
                AddTooltipForLastItem("Select this explicit local definition to inspect origin, dependencies, reverse dependencies, and related assumptions.");
                if (rowResult.bPressed) {
                    selectSemanticObject(ref);
                }
                consumePendingScroll(ref);
                ImGui::PopID();
            }
            if (!draftPreviewDefinitions.empty()) {
                ImGui::SeparatorText("Draft Materializations");
                for (const auto &definition : draftPreviewDefinitions) {
                    const auto ref = MakeDefinitionObjectRefV2(definition.DefinitionId);
                    const auto highlightState = ResolveHighlightState(selectionContext, *selectionOverview, ref);
                    ImGui::PushID(("draft." + definition.DefinitionId).c_str());
                    const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                        typesettingService,
                        "##ModelDraftDefinitionRow",
                        MakeMathRequest(RenderDialectDefinitionV2(definition, &model), baseFontSize * 1.02f),
                        MakeDraftDefinitionRowOptions(highlightState, baseFontSize));
                    AddTooltipForLastItem("Preview-only local definition inferred from the current draft. Select it to inspect provenance and dependencies before applying.");
                    if (rowResult.bPressed) {
                        selectSemanticObject(ref, false, true);
                    }
                    consumePendingScroll(ref);
                    ImGui::PopID();
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
                const auto ref = MakeRelationObjectRefV2(relation.RelationId);
                const auto highlightState = ResolveHighlightState(selectionContext, *selectionOverview, ref);
                ImGui::PushID(relation.RelationId.c_str());
                const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                    typesettingService,
                    "##ModelRelationRow",
                    MakeMathRequest(RenderDialectRelationV2(relation, &model), baseFontSize * 1.04f),
                    MakeRelationRowOptions(highlightState, baseFontSize));
                AddTooltipForLastItem("Select this relation to inspect referenced definitions, ambient vocabulary dependencies, spawned assumptions, and diagnostics.");
                if (rowResult.bPressed) {
                    selectSemanticObject(ref);
                }
                consumePendingScroll(ref);
                ImGui::PopID();
            }
            ImGui::EndChild();
            ImGui::EndChild();

            ImGui::TableNextColumn();
            ImGui::BeginChild("ModelFocusedDetailsPane", ImVec2(0.0f, 0.0f), true);
            ImGui::SeparatorText("Inspector");
            const auto inspectorNavigate = [&](const FSemanticObjectRefV2 &ref) {
                if (bSelectedModelSemanticObjectUsesDraftPreview) navigateToDraftSemanticObject(ref);
                else navigateToSemanticObject(ref);
            };
            if (const auto *selectedObject = selectionOverview->FindObject(SelectedModelSemanticObject); selectedObject != nullptr) {
                if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Definition) {
                    const auto *definition = FindDefinitionByIdV2(model, SelectedModelSemanticObject.ObjectId);
                    const auto *definitionReport = FindDefinitionReport(selectionOverview->Report, SelectedModelSemanticObject.ObjectId);
                    ModelV2::FModelEditorBufferV2 *editorBuffer = definition != nullptr
                        ? EnsureEditorBuffer(
                              ModelEditorBuffersByKey,
                              model,
                              EModelObjectKindV2::Definition,
                              SelectedModelSemanticObject.ObjectId)
                        : activeEditorBuffer;
                    const auto *definitionPreview =
                        editorBuffer != nullptr && editorBuffer->DefinitionPreview.has_value()
                            ? &(*editorBuffer->DefinitionPreview)
                            : nullptr;
                    const auto draftDefinition = definition == nullptr && bSelectedModelSemanticObjectUsesDraftPreview
                        ? FindDraftProposedDefinition(model, activeEditorBuffer, activeDraftOverview, SelectedModelSemanticObject.ObjectId)
                        : Slab::TOptional<ModelV2::FDefinitionV2>{};
                    if (definition != nullptr || draftDefinition.has_value()) {
                        DrawDefinitionInspector(
                            typesettingService,
                            model,
                            definition != nullptr ? *definition : *draftDefinition,
                            *selectedObject,
                            definitionReport,
                            definitionPreview,
                            inspectorTheme,
                            inspectorNavigate);
                    }
                } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Relation) {
                    const auto *relation = FindRelationByIdV2(model, SelectedModelSemanticObject.ObjectId);
                    const auto *relationReport = FindRelationReport(selectionOverview->Report, SelectedModelSemanticObject.ObjectId);
                    ModelV2::FModelEditorBufferV2 *editorBuffer = EnsureEditorBuffer(
                        ModelEditorBuffersByKey,
                        model,
                        EModelObjectKindV2::Relation,
                        SelectedModelSemanticObject.ObjectId);
                    const auto *relationPreview =
                        editorBuffer != nullptr && editorBuffer->RelationPreview.has_value()
                            ? &(*editorBuffer->RelationPreview)
                            : nullptr;
                    if (relation != nullptr) {
                        DrawRelationInspector(
                            typesettingService,
                            model,
                            *relation,
                            *selectedObject,
                            relationReport,
                            relationPreview,
                            inspectorTheme,
                            inspectorNavigate);
                    }
                } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::VocabularyEntry) {
                    if (const auto *entry = FindResolvedVocabularyEntry(resolvedVocabularyEntries, SelectedModelSemanticObject.ObjectId);
                        entry != nullptr) {
                        DrawVocabularyInspector(
                            typesettingService,
                            *entry,
                            *selectedObject,
                            inspectorTheme,
                            inspectorNavigate);
                    }
                } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Assumption) {
                    if (const auto *assumption = FindAssumptionReport(selectionOverview->Report, SelectedModelSemanticObject.ObjectId);
                        assumption != nullptr) {
                        const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(model, *assumption, selectionOverview);
                        DrawAssumptionInspector(
                            typesettingService,
                            model,
                            *assumption,
                            *selectedObject,
                            materializationPreview,
                            inspectorTheme,
                            inspectorNavigate);
                    }
                }
            } else {
                ImGui::TextDisabled("Select a vocabulary entry, definition, relation, or assumption.");
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

        auto *editorBuffer = activeEditorBuffer;
        if (editorBuffer == nullptr) {
            ImGui::TextDisabled("Select a definition or relation to edit.");
        } else {
            ImGui::TextDisabled(
                "Target: %s | Kind: %s | Source: %s",
                editorBuffer->TargetId.c_str(),
                ToString(editorBuffer->TargetKind),
                editorBuffer->bDraftDirty ? "draft" : "canonical");
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
                InvalidateModelWorkspaceViewState();
            }

            if (ImGui::Button("Parse / Preview")) {
                if (ParseEditorBufferPreviewV2(model, *editorBuffer)) {
                    ModelEditorStatus = "[Ok] Parsed draft for " + editorBuffer->TargetId + ".";
                } else {
                    ModelEditorStatus = "[Error] Could not parse draft for " + editorBuffer->TargetId + ".";
                }
                InvalidateModelWorkspaceViewState();
                bUiStateChangedThisFrame = true;
            }
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
                    if (editorBuffer->TargetKind == EModelObjectKindV2::Definition) {
                        selectSemanticObject(MakeDefinitionObjectRefV2(changeRecord.ObjectId));
                    } else if (editorBuffer->TargetKind == EModelObjectKindV2::Relation) {
                        selectSemanticObject(MakeRelationObjectRefV2(changeRecord.ObjectId));
                    }
                } else {
                    ModelEditorStatus = "[Error] Apply failed for " + editorBuffer->TargetId + ".";
                }
                InvalidateModelWorkspaceViewState();
                bUiStateChangedThisFrame = true;
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(!editorBuffer->bDraftDirty && !editorBuffer->bPreviewCurrent);
            if (ImGui::Button("Revert / Cancel")) {
                if (RevertEditorBufferV2(model, *editorBuffer)) {
                    ModelEditorStatus = "[Ok] Restored canonical notation for " + editorBuffer->TargetId + ".";
                    if (bSelectedModelSemanticObjectUsesDraftPreview) {
                        if (editorBuffer->TargetKind == EModelObjectKindV2::Definition) {
                            selectSemanticObject(MakeDefinitionObjectRefV2(editorBuffer->TargetId), false, false);
                        } else if (editorBuffer->TargetKind == EModelObjectKindV2::Relation) {
                            selectSemanticObject(MakeRelationObjectRefV2(editorBuffer->TargetId), false, false);
                        }
                    }
                }
                InvalidateModelWorkspaceViewState();
                bUiStateChangedThisFrame = true;
            }
            ImGui::EndDisabled();

            const auto acceptDraftAssumption = [&](const auto &assumption,
                                                   const auto &materializationPreview) -> bool {
                FModelChangeRecordV2 changeRecord;
                if (!AcceptAssumptionV2(model, assumption, &changeRecord)) return false;

                ModelLastChangeRecord = changeRecord;
                bModelHasLastChangeRecord = true;
                if (materializationPreview.ProposedDefinition.has_value()) {
                    ModelEditorStatus =
                        "[Ok] Materialized draft assumption '" + assumption.AssumptionId +
                        "' as '" + materializationPreview.ProposedDefinition->DefinitionId + "'.";
                } else {
                    ModelEditorStatus = "[Ok] Accepted draft assumption '" + assumption.AssumptionId + "'.";
                }

                ParseEditorBufferPreviewV2(model, *editorBuffer);
                InvalidateModelWorkspaceViewState();

                if (changeRecord.ObjectKind == EModelObjectKindV2::Definition && !changeRecord.ObjectId.empty()) {
                    ModelPendingScrollTarget = MakeDefinitionObjectRefV2(changeRecord.ObjectId);
                }
                return true;
            };

            const auto resolveDraftReferencedSymbol = [&](const auto &symbol, const auto *assumption) -> bool {
                const auto *previewOverview = GetEditorBufferPreviewOverview(editorBuffer);
                if (assumption != nullptr) {
                    const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(
                        model,
                        *assumption,
                        previewOverview);
                    if (materializationPreview.ProposedDefinition.has_value()) {
                        return acceptDraftAssumption(*assumption, materializationPreview);
                    }
                }

                PrefillModelNewDefinitionComposerForReferencedSymbol(
                    model,
                    symbol,
                    assumption,
                    previewOverview,
                    "[Info] Prefilled definition composer from unresolved draft symbol");
                return false;
            };

            if (bUiStateChangedThisFrame) {
                ImGui::TextDisabled("Refreshing preview state...");
            } else if (!editorBuffer->bPreviewCurrent) {
                ImGui::TextDisabled("Preview the current draft before applying.");
            } else if (editorBuffer->TargetKind == EModelObjectKindV2::Definition && editorBuffer->DefinitionPreview.has_value()) {
                const auto &preview = *editorBuffer->DefinitionPreview;
                if (!preview.bParseOk && preview.ParseError.has_value()) {
                    DrawRawDiagnosticList(preview.Diagnostics);
                } else {
                    ImGui::SeparatorText("Draft Interpretation");
                    Typesetting::ImGuiTypesetLabelValue(
                        typesettingService,
                        "Normalized",
                        MakeMathRequest(preview.NormalizedProjection, baseFontSize),
                        inspectorTheme.SecondaryTextOptions,
                        inspectorTheme.DetailMathOptions);
                    if (preview.InferredKind.has_value()) {
                        ImGui::BulletText("Inferred category: %s", ToString(*preview.InferredKind));
                    }
                    if (!preview.NormalizedInterpretation.empty()) {
                        ImGui::TextWrapped("%s", preview.NormalizedInterpretation.c_str());
                    }
                    if (preview.SemanticDelta.has_value()) {
                        DrawDraftSemanticDelta(*preview.SemanticDelta, navigateToDraftSemanticObject);
                    }
                    if (DrawReferencedSymbols(
                        preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                        preview.ReferencedSymbols,
                        &preview.Assumptions,
                        navigateToDraftSemanticObject,
                        resolveDraftReferencedSymbol)) {
                        bUiStateChangedThisFrame = true;
                    }
                    if (DrawPreviewDiagnosticSectionWithResolution(
                        preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                        MakeDefinitionObjectRefV2(preview.DefinitionId),
                        preview.Diagnostics,
                        preview.ReferencedSymbols,
                        &preview.Assumptions,
                        navigateToDraftSemanticObject,
                        resolveDraftReferencedSymbol)) {
                        bUiStateChangedThisFrame = true;
                    }
                    if (DrawDraftAssumptionWorkflowSummary(
                        model,
                        preview.Assumptions,
                        preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                        navigateToDraftSemanticObject,
                        acceptDraftAssumption)) {
                        bUiStateChangedThisFrame = true;
                    }
                }
            } else if (editorBuffer->TargetKind == EModelObjectKindV2::Relation && editorBuffer->RelationPreview.has_value()) {
                const auto &preview = *editorBuffer->RelationPreview;
                if (!preview.bParseOk && preview.ParseError.has_value()) {
                    DrawRawDiagnosticList(preview.Diagnostics);
                } else {
                    ImGui::SeparatorText("Draft Interpretation");
                    Typesetting::ImGuiTypesetLabelValue(
                        typesettingService,
                        "Normalized",
                        MakeMathRequest(preview.NormalizedProjection, baseFontSize),
                        inspectorTheme.SecondaryTextOptions,
                        inspectorTheme.DetailMathOptions);
                    ImGui::BulletText("Inferred category: %s", ToString(preview.InferredClass));
                    if (!preview.NormalizedInterpretation.empty()) {
                        ImGui::TextWrapped("%s", preview.NormalizedInterpretation.c_str());
                    }
                    if (preview.SemanticDelta.has_value()) {
                        DrawDraftSemanticDelta(*preview.SemanticDelta, navigateToDraftSemanticObject);
                    }
                    if (DrawReferencedSymbols(
                        preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                        preview.ReferencedSymbols,
                        &preview.Assumptions,
                        navigateToDraftSemanticObject,
                        resolveDraftReferencedSymbol)) {
                        bUiStateChangedThisFrame = true;
                    }
                    if (DrawPreviewDiagnosticSectionWithResolution(
                        preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                        MakeRelationObjectRefV2(preview.RelationId),
                        preview.Diagnostics,
                        preview.ReferencedSymbols,
                        &preview.Assumptions,
                        navigateToDraftSemanticObject,
                        resolveDraftReferencedSymbol)) {
                        bUiStateChangedThisFrame = true;
                    }
                    if (DrawDraftAssumptionWorkflowSummary(
                        model,
                        preview.Assumptions,
                        preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                        navigateToDraftSemanticObject,
                        acceptDraftAssumption)) {
                        bUiStateChangedThisFrame = true;
                    }
                }
            }
        }
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("ModelAssumptionsPane", ImVec2(0.0f, 0.0f), true);
        ImGui::SeparatorText("Assumed / Inferred Semantics");
        const bool bHasDraftPreviewAssumptions =
            draftPreviewAssumptions != nullptr && !draftPreviewAssumptions->empty();
        if (bUiStateChangedThisFrame) {
            ImGui::TextDisabled("Refreshing semantic state...");
        } else if (semanticReport.Assumptions.empty() && !bHasDraftPreviewAssumptions) {
            ImGui::TextDisabled("No implicit or accepted semantics are currently available.");
        } else {
            if (bHasDraftPreviewAssumptions) {
                ImGui::SeparatorText("Draft Preview");
                for (const auto &assumption : *draftPreviewAssumptions) {
                    const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(
                        model,
                        assumption,
                        activeDraftOverview != nullptr ? activeDraftOverview : &overview);
                    const auto ref = MakeAssumptionObjectRefV2(assumption.AssumptionId);
                    const auto highlightState = ResolveHighlightState(selectionContext, *selectionOverview, ref);
                    ImGui::PushID(("draft." + assumption.AssumptionId).c_str());
                    const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                        typesettingService,
                        "##ModelDraftAssumptionRow",
                        Typesetting::MakeTextRequest(
                            assumption.TargetSymbol.empty() ? assumption.Category : assumption.TargetSymbol,
                            Typesetting::FTypesetStyle{.FontPixelSize = baseFontSize * 0.95f}),
                        MakeDraftAssumptionRowOptions(highlightState, baseFontSize));
                    if (rowResult.bPressed) {
                        selectSemanticObject(ref, false, true);
                    }
                    consumePendingScroll(ref);
                    ImGui::TextDisabled(
                        "%s | source: %s | lifecycle: %s",
                        assumption.Category.c_str(),
                        assumption.SourceId.c_str(),
                        materializationPreview.LifecycleLabel.c_str());
                    if (!materializationPreview.SuggestedDefinitionId.empty()) {
                        ImGui::TextDisabled("Would create: %s", materializationPreview.SuggestedDefinitionId.c_str());
                    }
                    if (!assumption.Detail.empty()) {
                        ImGui::TextWrapped("%s", assumption.Detail.c_str());
                    }
                    ImGui::Separator();
                    ImGui::PopID();
                }
            }

            if (bHasDraftPreviewAssumptions && !semanticReport.Assumptions.empty()) {
                ImGui::SeparatorText("Canonical State");
            }

            for (const auto &assumption : semanticReport.Assumptions) {
                const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(model, assumption, &overview);
                const auto ref = MakeAssumptionObjectRefV2(assumption.AssumptionId);
                const auto highlightState = ResolveHighlightState(selectionContext, *selectionOverview, ref);
                ImGui::PushID(assumption.AssumptionId.c_str());
                const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                    typesettingService,
                    "##ModelAssumptionRow",
                    Typesetting::MakeTextRequest(
                        assumption.TargetSymbol.empty() ? assumption.Category : assumption.TargetSymbol,
                        Typesetting::FTypesetStyle{.FontPixelSize = baseFontSize * 0.95f}),
                    MakeAssumptionRowOptions(highlightState, baseFontSize));
                if (rowResult.bPressed) {
                    selectSemanticObject(ref);
                }
                consumePendingScroll(ref);
                ImGui::TextDisabled(
                    "%s | source: %s | status: %s",
                    assumption.Category.c_str(),
                    assumption.SourceId.c_str(),
                    ToString(assumption.Status));
                Slab::Str lifecycleSummary = materializationPreview.LifecycleLabel;
                if (!materializationPreview.SuggestedDefinitionId.empty()) {
                    lifecycleSummary += " | local artifact: " + materializationPreview.SuggestedDefinitionId;
                }
                ImGui::TextDisabled(
                    "Lifecycle: %s",
                    lifecycleSummary.c_str());
                if (!assumption.Detail.empty()) {
                    ImGui::TextWrapped("%s", assumption.Detail.c_str());
                }
                if (!materializationPreview.OutcomeLabel.empty()) {
                    ImGui::TextWrapped("%s", materializationPreview.OutcomeLabel.c_str());
                }
                if (assumption.DeclaredKind.has_value() || assumption.InferredKind.has_value()) {
                    ImGui::TextDisabled(
                        "Declared: %s | Inferred: %s",
                        assumption.DeclaredKind.has_value() ? ToString(*assumption.DeclaredKind) : "-",
                        assumption.InferredKind.has_value() ? ToString(*assumption.InferredKind) : "-");
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
                    const char *acceptLabel =
                        materializationPreview.ProposedDefinition.has_value() ? "Accept / Materialize" : "Accept";
                    if (ImGui::Button(acceptLabel)) {
                        FModelChangeRecordV2 changeRecord;
                        if (AcceptAssumptionV2(model, assumption, &changeRecord)) {
                            ModelLastChangeRecord = changeRecord;
                            bModelHasLastChangeRecord = true;
                            if (materializationPreview.ProposedDefinition.has_value()) {
                                ModelEditorStatus =
                                    "[Ok] Materialized semantic assumption '" + assumption.AssumptionId +
                                    "' as '" + materializationPreview.ProposedDefinition->DefinitionId + "'.";
                            } else {
                                ModelEditorStatus = "[Ok] Accepted semantic assumption '" + assumption.AssumptionId + "'.";
                            }
                            if (!changeRecord.ObjectId.empty() &&
                                changeRecord.ObjectKind == EModelObjectKindV2::Definition) {
                                selectSemanticObject(MakeDefinitionObjectRefV2(changeRecord.ObjectId), true);
                            } else {
                                selectSemanticObject(ref);
                            }
                            InvalidateModelWorkspaceViewState();
                            bUiStateChangedThisFrame = true;
                        }
                    }
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
                        bUiStateChangedThisFrame = true;
                    }
                }
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
                        bUiStateChangedThisFrame = true;
                    }
                }
                ImGui::EndDisabled();
                ImGui::Separator();
                ImGui::PopID();
                if (bUiStateChangedThisFrame) break;
            }
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }
    ImGui::EndChild();
}

auto FLabV2WindowManager::InvalidateModelWorkspaceViewState() -> void {
    CachedModelWorkspaceFrame = -1;
    bModelWorkspaceViewStateDirty = true;
    CachedModelWorkspaceViewState = {};
}

auto FLabV2WindowManager::PrefillModelNewDefinitionComposer(const Slab::Core::Model::V2::FModelV2 &model,
                                                            const Slab::Core::Model::V2::FDefinitionV2 &definition,
                                                            const Slab::Str &status) -> void {
    using namespace Slab::Core::Model::V2;

    ModelNewDefinitionId = definition.DefinitionId;
    ModelNewDefinitionDisplayName = definition.DisplayName;
    ModelNewDefinitionKind = definition.Kind;
    ModelNewDefinitionCoordinateRole =
        definition.Kind == EDefinitionKindV2::Coordinate ? definition.CoordinateRole : ECoordinateRoleV2::Generic;
    ModelNewDefinitionOperatorStyle = definition.OperatorStyle;
    ModelNewDefinitionNotation = RenderDialectDefinitionV2(definition, &model);
    ModelNewDefinitionStatus = status;
    ModelEditorStatus = status;
    ModelNewDefinitionPreview = PreviewNewDefinitionV2(
        model,
        ModelNewDefinitionId,
        ModelNewDefinitionKind,
        ModelNewDefinitionCoordinateRole,
        ModelNewDefinitionOperatorStyle,
        ModelNewDefinitionNotation,
        ModelNewDefinitionDisplayName);
    bShowModelNewDefinitionComposer = true;
    bShowWindowModelDefinitions = true;
    InvalidateModelWorkspaceViewState();
}

auto FLabV2WindowManager::PrefillModelNewDefinitionComposerForReferencedSymbol(
    const Slab::Core::Model::V2::FModelV2 &model,
    const Slab::Core::Model::V2::FReferencedSymbolSemanticV2 &symbol,
    const Slab::Core::Model::V2::FSemanticAssumptionV2 *assumption,
    const Slab::Core::Model::V2::FModelSemanticOverviewV2 *overview,
    const Slab::Str &statusPrefix) -> void {
    using namespace Slab::Core::Model::V2;

    if (assumption != nullptr) {
        const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(model, *assumption, overview);
        if (materializationPreview.ProposedDefinition.has_value()) {
            PrefillModelNewDefinitionComposer(
                model,
                *materializationPreview.ProposedDefinition,
                statusPrefix.empty()
                    ? "[Info] Prefilled definition composer from unresolved symbol '" + symbol.SymbolText + "'."
                    : statusPrefix + " '" + symbol.SymbolText + "'.");
            return;
        }
    }

    const auto definition = Detail::BuildDefinitionSuggestionForReferencedSymbolV2(model, symbol);
    PrefillModelNewDefinitionComposer(
        model,
        definition,
        statusPrefix.empty()
            ? "[Info] Prefilled definition composer from unresolved symbol '" + symbol.SymbolText + "'."
            : statusPrefix + " '" + symbol.SymbolText + "'.");
}

auto FLabV2WindowManager::SelectModelSemanticObject(const Slab::Core::Model::V2::FModelV2 &model,
                                                    const Slab::Core::Model::V2::FSemanticObjectRefV2 &ref,
                                                    const bool bRequestScroll,
                                                    const bool bUseDraftPreview) -> void {
    using namespace Slab::Core::Model::V2;

    if (!ref.IsValid()) return;

    SelectedModelSemanticObject = ref;
    bSelectedModelSemanticObjectUsesDraftPreview = bUseDraftPreview;
    if (bRequestScroll) ModelPendingScrollTarget = ref;

    switch (ref.Kind) {
        case ESemanticObjectKindV2::Definition:
            if (!bUseDraftPreview && FindDefinitionByIdV2(model, ref.ObjectId) != nullptr) {
                SelectedModelDefinitionId = ref.ObjectId;
                bSelectedModelDetailIsRelation = false;
            }
            break;
        case ESemanticObjectKindV2::Relation:
            if (!bUseDraftPreview && FindRelationByIdV2(model, ref.ObjectId) != nullptr) {
                SelectedModelRelationId = ref.ObjectId;
                bSelectedModelDetailIsRelation = true;
            }
            break;
        case ESemanticObjectKindV2::VocabularyEntry:
            SelectedModelVocabularyEntryId = ref.ObjectId;
            break;
        case ESemanticObjectKindV2::Assumption:
            SelectedModelAssumptionId = ref.ObjectId;
            break;
    }

    InvalidateModelWorkspaceViewState();
}

auto FLabV2WindowManager::PrepareModelWorkspaceViewState() -> FModelWorkspaceViewState {
    using namespace Slab::Core::Model::V2;

    const auto frameIndex = ImGui::GetFrameCount();
    if (!bModelWorkspaceViewStateDirty && CachedModelWorkspaceFrame == frameIndex) {
        return CachedModelWorkspaceViewState;
    }

    FModelWorkspaceViewState state;

    if (ModelDemoCatalog.empty()) {
        ModelDemoCatalog = BuildDemoModelsV2();
    }
    if (ModelDemoCatalog.empty()) return state;

    if (SelectedModelIndex < 0 || SelectedModelIndex >= static_cast<int>(ModelDemoCatalog.size())) {
        SelectedModelIndex = 0;
    }

    auto &model = ModelDemoCatalog[SelectedModelIndex];
    const auto &vocabularyPresets = GetBaseVocabularyPresetCatalogV2();
    if ((model.BaseVocabulary.ActivePresetId.empty() ||
         FindBaseVocabularyPresetByIdV2(model.BaseVocabulary.ActivePresetId) == nullptr) &&
        !vocabularyPresets.empty()) {
        model.BaseVocabulary.ActivePresetId = vocabularyPresets.front().PresetId;
    }

    state.ActiveVocabularyPreset = FindBaseVocabularyPresetByIdV2(model.BaseVocabulary.ActivePresetId);
    if (state.ActiveVocabularyPreset == nullptr && !vocabularyPresets.empty()) {
        state.ActiveVocabularyPreset = &vocabularyPresets.front();
        model.BaseVocabulary.ActivePresetId = state.ActiveVocabularyPreset->PresetId;
    }

    state.Overview = BuildModelSemanticOverviewV2(model);
    const auto &semanticReport = state.Overview.Report;
    const auto &resolvedVocabularyEntries = state.Overview.VocabularyEntries;

    if (SelectedModelVocabularyEntryId.empty() && !resolvedVocabularyEntries.empty()) {
        SelectedModelVocabularyEntryId = resolvedVocabularyEntries.front().Entry.EntryId;
    }
    if (!SelectedModelVocabularyEntryId.empty() &&
        FindResolvedVocabularyEntry(resolvedVocabularyEntries, SelectedModelVocabularyEntryId) == nullptr &&
        !resolvedVocabularyEntries.empty()) {
        SelectedModelVocabularyEntryId = resolvedVocabularyEntries.front().Entry.EntryId;
    }

    if (SelectedModelDefinitionId.empty() && !model.Definitions.empty()) {
        SelectedModelDefinitionId = model.Definitions.front().DefinitionId;
    }
    if (SelectedModelRelationId.empty() && !model.Relations.empty()) {
        SelectedModelRelationId = model.Relations.front().RelationId;
    }
    if (!SelectedModelDefinitionId.empty() &&
        FindDefinitionByIdV2(model, SelectedModelDefinitionId) == nullptr &&
        !model.Definitions.empty()) {
        SelectedModelDefinitionId = model.Definitions.front().DefinitionId;
    }
    if (!SelectedModelRelationId.empty() &&
        FindRelationByIdV2(model, SelectedModelRelationId) == nullptr &&
        !model.Relations.empty()) {
        SelectedModelRelationId = model.Relations.front().RelationId;
    }

    const auto editorTargetKind = bSelectedModelDetailIsRelation ? EModelObjectKindV2::Relation : EModelObjectKindV2::Definition;
    const auto editorTargetId = bSelectedModelDetailIsRelation ? SelectedModelRelationId : SelectedModelDefinitionId;
    state.ActiveEditorBuffer = EnsureEditorBuffer(ModelEditorBuffersByKey, model, editorTargetKind, editorTargetId);
    state.ActiveDraftOverview = GetEditorBufferPreviewOverview(state.ActiveEditorBuffer);
    state.SelectionOverview = &state.Overview;

    if (SelectedModelSemanticObject.IsValid()) {
        const bool bSelectionIsActiveEditorTarget =
            (editorTargetKind == EModelObjectKindV2::Definition &&
             SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Definition &&
             SelectedModelSemanticObject.ObjectId == editorTargetId) ||
            (editorTargetKind == EModelObjectKindV2::Relation &&
             SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Relation &&
             SelectedModelSemanticObject.ObjectId == editorTargetId);

        if (state.ActiveDraftOverview != nullptr &&
            (bSelectedModelSemanticObjectUsesDraftPreview || bSelectionIsActiveEditorTarget) &&
            state.ActiveDraftOverview->FindObject(SelectedModelSemanticObject) != nullptr) {
            state.SelectionOverview = state.ActiveDraftOverview;
            bSelectedModelSemanticObjectUsesDraftPreview =
                bSelectedModelSemanticObjectUsesDraftPreview || bSelectionIsActiveEditorTarget;
        } else if (state.Overview.FindObject(SelectedModelSemanticObject) != nullptr) {
            bSelectedModelSemanticObjectUsesDraftPreview = false;
        } else if (state.ActiveDraftOverview != nullptr &&
                   state.ActiveDraftOverview->FindObject(SelectedModelSemanticObject) != nullptr) {
            bSelectedModelSemanticObjectUsesDraftPreview = true;
            state.SelectionOverview = state.ActiveDraftOverview;
        } else {
            SelectedModelSemanticObject = {};
            bSelectedModelSemanticObjectUsesDraftPreview = false;
        }
    }

    if (!SelectedModelSemanticObject.IsValid()) {
        if (!SelectedModelDefinitionId.empty() && FindDefinitionByIdV2(model, SelectedModelDefinitionId) != nullptr) {
            SelectModelSemanticObject(model, MakeDefinitionObjectRefV2(SelectedModelDefinitionId), false, false);
        } else if (!SelectedModelRelationId.empty() && FindRelationByIdV2(model, SelectedModelRelationId) != nullptr) {
            SelectModelSemanticObject(model, MakeRelationObjectRefV2(SelectedModelRelationId), false, false);
        } else if (!SelectedModelVocabularyEntryId.empty()) {
            SelectModelSemanticObject(model, MakeVocabularyObjectRefV2(SelectedModelVocabularyEntryId), false, false);
        } else if (!semanticReport.Assumptions.empty()) {
            SelectModelSemanticObject(model, MakeAssumptionObjectRefV2(semanticReport.Assumptions.front().AssumptionId), false, false);
        }
    }
    if (!SelectedModelSemanticObject.IsValid() && !resolvedVocabularyEntries.empty()) {
        SelectModelSemanticObject(model, MakeVocabularyObjectRefV2(resolvedVocabularyEntries.front().Entry.EntryId), false, false);
    }

    if (SelectedModelSemanticObject.IsValid() &&
        bSelectedModelSemanticObjectUsesDraftPreview &&
        state.ActiveDraftOverview != nullptr &&
        state.ActiveDraftOverview->FindObject(SelectedModelSemanticObject) != nullptr) {
        state.SelectionOverview = state.ActiveDraftOverview;
    } else {
        state.SelectionOverview = &state.Overview;
        bSelectedModelSemanticObjectUsesDraftPreview = false;
    }

    state.SelectionContext = BuildSemanticSelectionContextV2(*state.SelectionOverview, SelectedModelSemanticObject);
    state.DraftPreviewDefinitions = CollectDraftProposedDefinitions(model, state.ActiveEditorBuffer, state.ActiveDraftOverview);
    state.DraftPreviewAssumptions = GetEditorBufferPreviewAssumptions(state.ActiveEditorBuffer);
    state.Model = &model;
    state.bAvailable = true;
    const bool bSelectionUsesDraftOverview =
        state.ActiveDraftOverview != nullptr && state.SelectionOverview == state.ActiveDraftOverview;
    SyncModelSemanticGraphWindow(model, *state.SelectionOverview, bSelectionUsesDraftOverview);
    CachedModelWorkspaceViewState = state;
    CachedModelWorkspaceViewState.SelectionOverview =
        bSelectionUsesDraftOverview && CachedModelWorkspaceViewState.ActiveDraftOverview != nullptr
            ? CachedModelWorkspaceViewState.ActiveDraftOverview
            : &CachedModelWorkspaceViewState.Overview;
    CachedModelWorkspaceFrame = frameIndex;
    bModelWorkspaceViewStateDirty = false;
    return CachedModelWorkspaceViewState;
}

auto FLabV2WindowManager::DrawModelVocabularyPanel() -> void {
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

    const auto resetModelWorkspaceState = [&]() {
        SelectedModelSemanticObject = {};
        bSelectedModelSemanticObjectUsesDraftPreview = false;
        ModelPendingScrollTarget = {};
        SelectedModelVocabularyEntryId.clear();
        SelectedModelDefinitionId.clear();
        SelectedModelRelationId.clear();
        SelectedModelAssumptionId.clear();
        bSelectedModelDetailIsRelation = false;
        ModelEditorBuffersByKey.clear();
        ModelEditorStatus.clear();
        bModelHasLastChangeRecord = false;
        ModelNewDefinitionPreview.reset();
        ModelNewRelationPreview.reset();
        ModelNewDefinitionStatus.clear();
        ModelNewRelationStatus.clear();
        InvalidateModelWorkspaceViewState();
    };

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
    AddTooltipForLastItem("Switch between seeded reference models.");
    if (bModelChanged) {
        resetModelWorkspaceState();
    }

    auto &model = ModelDemoCatalog[SelectedModelIndex];
    const auto &vocabularyPresets = GetBaseVocabularyPresetCatalogV2();
    if ((model.BaseVocabulary.ActivePresetId.empty() ||
         FindBaseVocabularyPresetByIdV2(model.BaseVocabulary.ActivePresetId) == nullptr) &&
        !vocabularyPresets.empty()) {
        model.BaseVocabulary.ActivePresetId = vocabularyPresets.front().PresetId;
    }

    bool bVocabularyChanged = false;
    const auto *activeVocabularyPreset = FindBaseVocabularyPresetByIdV2(model.BaseVocabulary.ActivePresetId);
    if (activeVocabularyPreset == nullptr && !vocabularyPresets.empty()) {
        activeVocabularyPreset = &vocabularyPresets.front();
        model.BaseVocabulary.ActivePresetId = activeVocabularyPreset->PresetId;
    }

    if (ImGui::BeginCombo(
            "Base Vocabulary",
            activeVocabularyPreset != nullptr ? activeVocabularyPreset->Name.c_str() : "<none>")) {
        for (const auto &preset : vocabularyPresets) {
            const bool bSelected = preset.PresetId == model.BaseVocabulary.ActivePresetId;
            if (ImGui::Selectable(preset.Name.c_str(), bSelected)) {
                model.BaseVocabulary.ActivePresetId = preset.PresetId;
                bVocabularyChanged = true;
            }
            if (bSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (bVocabularyChanged) {
        resetModelWorkspaceState();
        ModelEditorStatus = "[Ok] Switched base vocabulary to '" + model.BaseVocabulary.ActivePresetId + "'.";
    }

    const auto state = PrepareModelWorkspaceViewState();
    if (!state.bAvailable || state.Model == nullptr) {
        ImGui::TextDisabled("No model state available.");
        return;
    }

    auto &typesettingService = *UiTypesettingService;
    const auto baseFontSize = ImGui::GetFontSize();
    ImGui::TextDisabled("Ambient / Readonly");
    if (state.ActiveVocabularyPreset != nullptr) {
        ImGui::TextDisabled("%s (%s)", state.ActiveVocabularyPreset->Name.c_str(), state.ActiveVocabularyPreset->PresetId.c_str());
    }
    ImGui::SeparatorText("Model Status");
    DrawSemanticSummaryBadge("Parse", ToString(state.Overview.Status.ParseHealth), HealthTint(state.Overview.Status.ParseHealth));
    ImGui::SameLine();
    DrawSemanticSummaryBadge("Semantic", ToString(state.Overview.Status.SemanticHealth), HealthTint(state.Overview.Status.SemanticHealth));
    ImGui::SameLine();
    ImGui::TextDisabled("Unresolved: %zu", state.Overview.Status.UnresolvedSymbolCount);
    ImGui::Dummy(ImVec2(0.0f, baseFontSize * 0.35f));

    const auto consumePendingScroll = [&](const FSemanticObjectRefV2 &ref) {
        if (!ModelPendingScrollTarget.IsValid()) return;
        if (!AreSemanticObjectRefsEqualV2(ModelPendingScrollTarget, ref)) return;
        ImGui::SetScrollHereY(0.35f);
        ModelPendingScrollTarget = {};
    };

    for (const auto &entry : state.Overview.VocabularyEntries) {
        const auto ref = MakeVocabularyObjectRefV2(entry.Entry.EntryId);
        const auto highlightState = ResolveHighlightState(state.SelectionContext, *state.SelectionOverview, ref);
        ImGui::PushID(entry.Entry.EntryId.c_str());
        const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
            typesettingService,
            "##ModelVocabularyEntryRow",
            MakeMathRequest(RenderBaseVocabularyEntryLatexV2(entry.Entry), baseFontSize * 0.98f),
            MakeVocabularyRowOptions(highlightState, baseFontSize));
        if (rowResult.bPressed) {
            SelectModelSemanticObject(*state.Model, ref, false, false);
        }
        consumePendingScroll(ref);
        ImGui::PopID();
    }
}

auto FLabV2WindowManager::DrawModelDefinitionsPanel() -> void {
    using namespace Slab::Core::Model::V2;

    auto state = PrepareModelWorkspaceViewState();
    if (!state.bAvailable || state.Model == nullptr) {
        ImGui::TextDisabled("No model state available.");
        return;
    }

    auto &model = *state.Model;
    auto &typesettingService = *UiTypesettingService;
    const auto baseFontSize = ImGui::GetFontSize();
    const auto navigateToCanonicalSemanticObject = [&](const FSemanticObjectRefV2 &ref) {
        if (state.Overview.FindObject(ref) == nullptr) return;
        SelectModelSemanticObject(model, ref, true, false);
    };
    const auto resolveComposerReferencedSymbol = [&](const auto &symbol, const auto *assumption) -> bool {
        PrefillModelNewDefinitionComposerForReferencedSymbol(
            model,
            symbol,
            assumption,
            ModelNewRelationPreview.has_value() && ModelNewRelationPreview->SemanticOverview.has_value()
                ? &(*ModelNewRelationPreview->SemanticOverview)
                : nullptr,
            "[Info] Prefilled definition composer from unresolved new-relation symbol");
        return false;
    };
    const auto refreshActiveEditorPreview = [&]() {
        if (state.ActiveEditorBuffer == nullptr || !state.ActiveEditorBuffer->bPreviewCurrent) return;
        ParseEditorBufferPreviewV2(model, *state.ActiveEditorBuffer);
        InvalidateModelWorkspaceViewState();
    };

    if (ImGui::Button(bShowModelNewDefinitionComposer ? "Close Composer" : "New Definition")) {
        bShowModelNewDefinitionComposer = !bShowModelNewDefinitionComposer;
    }
    if (bShowModelNewDefinitionComposer) {
        ImGui::SeparatorText("Create Definition");
        char idBuffer[256];
        SetTextBufferFromString(ModelNewDefinitionId, idBuffer, sizeof(idBuffer));
        if (ImGui::InputText("Definition Id", idBuffer, sizeof(idBuffer))) {
            ModelNewDefinitionId = idBuffer;
            ModelNewDefinitionPreview.reset();
        }

        char displayBuffer[256];
        SetTextBufferFromString(ModelNewDefinitionDisplayName, displayBuffer, sizeof(displayBuffer));
        if (ImGui::InputText("Display Name", displayBuffer, sizeof(displayBuffer))) {
            ModelNewDefinitionDisplayName = displayBuffer;
            ModelNewDefinitionPreview.reset();
        }

        constexpr EDefinitionKindV2 DefinitionKinds[] = {
            EDefinitionKindV2::ScalarParameter,
            EDefinitionKindV2::Coordinate,
            EDefinitionKindV2::StateVariable,
            EDefinitionKindV2::Field,
            EDefinitionKindV2::OperatorSymbol,
            EDefinitionKindV2::ObservableSymbol
        };
        if (ImGui::BeginCombo("Kind", ToString(ModelNewDefinitionKind))) {
            for (const auto definitionKind : DefinitionKinds) {
                const bool bSelected = definitionKind == ModelNewDefinitionKind;
                if (ImGui::Selectable(ToString(definitionKind), bSelected)) {
                    ModelNewDefinitionKind = definitionKind;
                    ModelNewDefinitionPreview.reset();
                }
                if (bSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        char notationBuffer[4096];
        SetTextBufferFromString(ModelNewDefinitionNotation, notationBuffer, sizeof(notationBuffer));
        if (ImGui::InputTextMultiline(
                "##ModelNewDefinitionNotation",
                notationBuffer,
                sizeof(notationBuffer),
                ImVec2(-FLT_MIN, 100.0f),
                ImGuiInputTextFlags_AllowTabInput)) {
            ModelNewDefinitionNotation = notationBuffer;
            ModelNewDefinitionPreview.reset();
        }

        const auto refreshDefinitionPreview = [&]() {
            ModelNewDefinitionPreview = PreviewNewDefinitionV2(
                model,
                ModelNewDefinitionId,
                ModelNewDefinitionKind,
                ModelNewDefinitionCoordinateRole,
                ModelNewDefinitionOperatorStyle,
                ModelNewDefinitionNotation,
                ModelNewDefinitionDisplayName);
            ModelNewDefinitionStatus = ModelNewDefinitionPreview->bCanApply
                ? "[Ok] Previewed new definition '" + ModelNewDefinitionId + "'."
                : "[Error] New definition preview has blocking issues.";
        };

        if (ImGui::Button("Preview Create")) {
            refreshDefinitionPreview();
        }
        ImGui::SameLine();
        if (ImGui::Button("Create Definition")) {
            if (!ModelNewDefinitionPreview.has_value()) refreshDefinitionPreview();
            FModelChangeRecordV2 changeRecord;
            if (ModelNewDefinitionPreview.has_value() &&
                ApplyNewDefinitionPreviewV2(model, *ModelNewDefinitionPreview, &changeRecord)) {
                ModelLastChangeRecord = changeRecord;
                bModelHasLastChangeRecord = true;
                ModelNewDefinitionStatus = "[Ok] Created definition '" + changeRecord.ObjectId + "'.";
                refreshActiveEditorPreview();
                if (state.ActiveEditorBuffer != nullptr &&
                    state.ActiveEditorBuffer->bPreviewCurrent &&
                    state.ActiveEditorBuffer->TargetId != changeRecord.ObjectId) {
                    ModelPendingScrollTarget = MakeDefinitionObjectRefV2(changeRecord.ObjectId);
                } else {
                    SelectModelSemanticObject(model, MakeDefinitionObjectRefV2(changeRecord.ObjectId), true, false);
                }
                bShowModelNewDefinitionComposer = false;
                ModelNewDefinitionPreview.reset();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel Create")) {
            bShowModelNewDefinitionComposer = false;
            ModelNewDefinitionPreview.reset();
            ModelNewDefinitionStatus.clear();
        }
        if (!ModelNewDefinitionStatus.empty()) {
            ImGui::TextWrapped("%s", ModelNewDefinitionStatus.c_str());
        }
        if (ModelNewDefinitionPreview.has_value()) {
            DrawRawDiagnosticList(ModelNewDefinitionPreview->Diagnostics);
        }
        ImGui::Separator();
    }

    const auto consumePendingScroll = [&](const FSemanticObjectRefV2 &ref) {
        if (!ModelPendingScrollTarget.IsValid()) return;
        if (!AreSemanticObjectRefsEqualV2(ModelPendingScrollTarget, ref)) return;
        ImGui::SetScrollHereY(0.35f);
        ModelPendingScrollTarget = {};
    };

    for (const auto &definition : model.Definitions) {
        const auto ref = MakeDefinitionObjectRefV2(definition.DefinitionId);
        const auto highlightState = ResolveHighlightState(state.SelectionContext, *state.SelectionOverview, ref);
        ImGui::PushID(definition.DefinitionId.c_str());
        const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
            typesettingService,
            "##ModelDefinitionRow",
            MakeMathRequest(RenderDialectDefinitionV2(definition, &model), baseFontSize * 1.02f),
            MakeDefinitionRowOptions(highlightState, baseFontSize));
        if (rowResult.bPressed) {
            SelectModelSemanticObject(model, ref, false, false);
        }
        consumePendingScroll(ref);
        ImGui::PopID();
    }

    if (!state.DraftPreviewDefinitions.empty()) {
        ImGui::SeparatorText("Draft Materializations");
        for (const auto &definition : state.DraftPreviewDefinitions) {
            const auto ref = MakeDefinitionObjectRefV2(definition.DefinitionId);
            const auto highlightState = ResolveHighlightState(state.SelectionContext, *state.SelectionOverview, ref);
            ImGui::PushID(("draft." + definition.DefinitionId).c_str());
            const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                typesettingService,
                "##ModelDraftDefinitionRow",
                MakeMathRequest(RenderDialectDefinitionV2(definition, &model), baseFontSize * 1.02f),
                MakeDraftDefinitionRowOptions(highlightState, baseFontSize));
            if (rowResult.bPressed) {
                SelectModelSemanticObject(model, ref, false, true);
            }
            consumePendingScroll(ref);
            ImGui::PopID();
        }
    }
}

auto FLabV2WindowManager::DrawModelRelationsPanel() -> void {
    using namespace Slab::Core::Model::V2;

    auto state = PrepareModelWorkspaceViewState();
    if (!state.bAvailable || state.Model == nullptr) {
        ImGui::TextDisabled("No model state available.");
        return;
    }

    auto &model = *state.Model;
    auto &typesettingService = *UiTypesettingService;
    const auto baseFontSize = ImGui::GetFontSize();
    const auto navigateToCanonicalSemanticObject = [&](const FSemanticObjectRefV2 &ref) {
        if (state.Overview.FindObject(ref) == nullptr) return;
        SelectModelSemanticObject(model, ref, true, false);
    };
    const auto resolveComposerReferencedSymbol = [&](const auto &symbol, const auto *assumption) -> bool {
        PrefillModelNewDefinitionComposerForReferencedSymbol(
            model,
            symbol,
            assumption,
            ModelNewRelationPreview.has_value() && ModelNewRelationPreview->SemanticOverview.has_value()
                ? &(*ModelNewRelationPreview->SemanticOverview)
                : nullptr,
            "[Info] Prefilled definition composer from unresolved new-relation symbol");
        return false;
    };

    if (ImGui::Button(bShowModelNewRelationComposer ? "Close Composer" : "New Relation")) {
        bShowModelNewRelationComposer = !bShowModelNewRelationComposer;
    }
    if (bShowModelNewRelationComposer) {
        ImGui::SeparatorText("Create Relation");
        char idBuffer[256];
        SetTextBufferFromString(ModelNewRelationId, idBuffer, sizeof(idBuffer));
        if (ImGui::InputText("Relation Id", idBuffer, sizeof(idBuffer))) {
            ModelNewRelationId = idBuffer;
            ModelNewRelationPreview.reset();
        }

        char nameBuffer[256];
        SetTextBufferFromString(ModelNewRelationName, nameBuffer, sizeof(nameBuffer));
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            ModelNewRelationName = nameBuffer;
            ModelNewRelationPreview.reset();
        }

        constexpr ERelationKindV2 RelationKinds[] = {
            ERelationKindV2::Equation,
            ERelationKindV2::DifferentialEquation,
            ERelationKindV2::OperatorEquation,
            ERelationKindV2::Constraint,
            ERelationKindV2::Identity,
            ERelationKindV2::SymbolicCondition
        };
        if (ImGui::BeginCombo("Relation Kind", ToString(ModelNewRelationKind))) {
            for (const auto relationKind : RelationKinds) {
                const bool bSelected = relationKind == ModelNewRelationKind;
                if (ImGui::Selectable(ToString(relationKind), bSelected)) {
                    ModelNewRelationKind = relationKind;
                    ModelNewRelationPreview.reset();
                }
                if (bSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        char notationBuffer[4096];
        SetTextBufferFromString(ModelNewRelationNotation, notationBuffer, sizeof(notationBuffer));
        if (ImGui::InputTextMultiline(
                "##ModelNewRelationNotation",
                notationBuffer,
                sizeof(notationBuffer),
                ImVec2(-FLT_MIN, 120.0f),
                ImGuiInputTextFlags_AllowTabInput)) {
            ModelNewRelationNotation = notationBuffer;
            ModelNewRelationPreview.reset();
        }

        const auto refreshRelationPreview = [&]() {
            ModelNewRelationPreview = PreviewNewRelationV2(
                model,
                ModelNewRelationId,
                ModelNewRelationKind,
                ModelNewRelationNotation,
                ModelNewRelationName);
            ModelNewRelationStatus = ModelNewRelationPreview->bCanApply
                ? "[Ok] Previewed new relation '" + ModelNewRelationId + "'."
                : "[Error] New relation preview has blocking issues.";
        };

        if (ImGui::Button("Preview Create")) {
            refreshRelationPreview();
        }
        ImGui::SameLine();
        if (ImGui::Button("Create Relation")) {
            if (!ModelNewRelationPreview.has_value()) refreshRelationPreview();
            FModelChangeRecordV2 changeRecord;
            if (ModelNewRelationPreview.has_value() &&
                ApplyNewRelationPreviewV2(model, *ModelNewRelationPreview, &changeRecord)) {
                ModelLastChangeRecord = changeRecord;
                bModelHasLastChangeRecord = true;
                ModelNewRelationStatus = "[Ok] Created relation '" + changeRecord.ObjectId + "'.";
                SelectModelSemanticObject(model, MakeRelationObjectRefV2(changeRecord.ObjectId), true, false);
                bShowModelNewRelationComposer = false;
                ModelNewRelationPreview.reset();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel Create")) {
            bShowModelNewRelationComposer = false;
            ModelNewRelationPreview.reset();
            ModelNewRelationStatus.clear();
        }
        if (!ModelNewRelationStatus.empty()) {
            ImGui::TextWrapped("%s", ModelNewRelationStatus.c_str());
        }
        if (ModelNewRelationPreview.has_value()) {
            const auto &preview = *ModelNewRelationPreview;
            if (!preview.bParseOk && preview.ParseError.has_value()) {
                DrawRawDiagnosticList(preview.Diagnostics);
            } else {
                if (preview.SemanticDelta.has_value()) {
                    DrawDraftSemanticDelta(*preview.SemanticDelta, navigateToCanonicalSemanticObject);
                }
                DrawReferencedSymbols(
                    preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                    preview.ReferencedSymbols,
                    &preview.Assumptions,
                    navigateToCanonicalSemanticObject,
                    resolveComposerReferencedSymbol);
                DrawPreviewDiagnosticSectionWithResolution(
                    preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                    MakeRelationObjectRefV2(preview.RelationId),
                    preview.Diagnostics,
                    preview.ReferencedSymbols,
                    &preview.Assumptions,
                    navigateToCanonicalSemanticObject,
                    resolveComposerReferencedSymbol);
            }
        }
        ImGui::Separator();
    }

    const auto consumePendingScroll = [&](const FSemanticObjectRefV2 &ref) {
        if (!ModelPendingScrollTarget.IsValid()) return;
        if (!AreSemanticObjectRefsEqualV2(ModelPendingScrollTarget, ref)) return;
        ImGui::SetScrollHereY(0.35f);
        ModelPendingScrollTarget = {};
    };

    for (const auto &relation : model.Relations) {
        const auto ref = MakeRelationObjectRefV2(relation.RelationId);
        const auto highlightState = ResolveHighlightState(state.SelectionContext, *state.SelectionOverview, ref);
        ImGui::PushID(relation.RelationId.c_str());
        const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
            typesettingService,
            "##ModelRelationRow",
            MakeMathRequest(RenderDialectRelationV2(relation, &model), baseFontSize * 1.04f),
            MakeRelationRowOptions(highlightState, baseFontSize));
        if (rowResult.bPressed) {
            SelectModelSemanticObject(model, ref, false, false);
        }
        consumePendingScroll(ref);
        ImGui::PopID();
    }
}

auto FLabV2WindowManager::DrawModelEditorPanel() -> void {
    using namespace Slab::Core::Model::V2;

    auto state = PrepareModelWorkspaceViewState();
    if (!state.bAvailable || state.Model == nullptr) {
        ImGui::TextDisabled("No model state available.");
        return;
    }

    auto &model = *state.Model;
    auto *editorBuffer = state.ActiveEditorBuffer;
    if (editorBuffer == nullptr) {
        ImGui::TextDisabled("Select a definition or relation to edit.");
        return;
    }

    auto &typesettingService = *UiTypesettingService;
    const auto baseFontSize = ImGui::GetFontSize();
    FModelInspectorTheme inspectorTheme;
    inspectorTheme.BaseFontSize = baseFontSize;
    inspectorTheme.SecondaryTextOptions.Style.FontPixelSize = baseFontSize * 0.92f;
    inspectorTheme.SecondaryTextOptions.Tint = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    inspectorTheme.DetailMathOptions.Style.FontPixelSize = baseFontSize * 1.12f;
    inspectorTheme.DetailMathOptions.Tint = ImGui::GetColorU32(ImGuiCol_Text);

    const auto navigateToDraftSemanticObject = [&](const FSemanticObjectRefV2 &ref) {
        SelectModelSemanticObject(model, ref, true, true);
    };

    const auto acceptDraftAssumption = [&](const auto &assumption,
                                           const auto &materializationPreview) -> bool {
        FModelChangeRecordV2 changeRecord;
        if (!AcceptAssumptionV2(model, assumption, &changeRecord)) return false;

        ModelLastChangeRecord = changeRecord;
        bModelHasLastChangeRecord = true;
        if (materializationPreview.ProposedDefinition.has_value()) {
            ModelEditorStatus =
                "[Ok] Materialized draft assumption '" + assumption.AssumptionId +
                "' as '" + materializationPreview.ProposedDefinition->DefinitionId + "'.";
        } else {
            ModelEditorStatus = "[Ok] Accepted draft assumption '" + assumption.AssumptionId + "'.";
        }

        ParseEditorBufferPreviewV2(model, *editorBuffer);
        InvalidateModelWorkspaceViewState();

        if (changeRecord.ObjectKind == EModelObjectKindV2::Definition && !changeRecord.ObjectId.empty()) {
            ModelPendingScrollTarget = MakeDefinitionObjectRefV2(changeRecord.ObjectId);
        }
        return true;
    };

    const auto resolveDraftReferencedSymbol = [&](const auto &symbol, const auto *assumption) -> bool {
        const auto *previewOverview = GetEditorBufferPreviewOverview(editorBuffer);
        if (assumption != nullptr) {
            const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(
                model,
                *assumption,
                previewOverview);
            if (materializationPreview.ProposedDefinition.has_value()) {
                return acceptDraftAssumption(*assumption, materializationPreview);
            }
        }

        PrefillModelNewDefinitionComposerForReferencedSymbol(
            model,
            symbol,
            assumption,
            previewOverview,
            "[Info] Prefilled definition composer from unresolved draft symbol");
        return false;
    };

    bool bUiStateChangedThisFrame = false;
    ImGui::TextDisabled(
        "Target: %s | Kind: %s | Source: %s",
        editorBuffer->TargetId.c_str(),
        ToString(editorBuffer->TargetKind),
        editorBuffer->bDraftDirty ? "draft" : "canonical");
    Typesetting::ImGuiTypesetMath(typesettingService, editorBuffer->CanonicalNotation, inspectorTheme.DetailMathOptions);

    char buffer[4096];
    SetTextBufferFromString(editorBuffer->DraftNotation, buffer, sizeof(buffer));
    if (ImGui::InputTextMultiline(
            "##ModelEditorDraftNotation",
            buffer,
            sizeof(buffer),
            ImVec2(-FLT_MIN, 120.0f),
            ImGuiInputTextFlags_AllowTabInput)) {
        SetEditorBufferDraftV2(*editorBuffer, buffer);
        InvalidateModelWorkspaceViewState();
    }

    if (ImGui::Button("Parse / Preview")) {
        if (ParseEditorBufferPreviewV2(model, *editorBuffer)) {
            ModelEditorStatus = "[Ok] Parsed draft for " + editorBuffer->TargetId + ".";
        } else {
            ModelEditorStatus = "[Error] Could not parse draft for " + editorBuffer->TargetId + ".";
        }
        InvalidateModelWorkspaceViewState();
        bUiStateChangedThisFrame = true;
    }
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
            if (editorBuffer->TargetKind == EModelObjectKindV2::Definition) {
                SelectModelSemanticObject(model, MakeDefinitionObjectRefV2(changeRecord.ObjectId), false, false);
            } else {
                SelectModelSemanticObject(model, MakeRelationObjectRefV2(changeRecord.ObjectId), false, false);
            }
        }
        InvalidateModelWorkspaceViewState();
        bUiStateChangedThisFrame = true;
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!editorBuffer->bDraftDirty && !editorBuffer->bPreviewCurrent);
    if (ImGui::Button("Revert / Cancel")) {
        RevertEditorBufferV2(model, *editorBuffer);
        InvalidateModelWorkspaceViewState();
        bUiStateChangedThisFrame = true;
    }
    ImGui::EndDisabled();

    if (!ModelEditorStatus.empty()) {
        ImGui::TextWrapped("%s", ModelEditorStatus.c_str());
    }

    if (bUiStateChangedThisFrame || !editorBuffer->bPreviewCurrent) {
        return;
    }

    if (editorBuffer->TargetKind == EModelObjectKindV2::Definition && editorBuffer->DefinitionPreview.has_value()) {
        const auto &preview = *editorBuffer->DefinitionPreview;
        if (!preview.bParseOk && preview.ParseError.has_value()) {
            DrawRawDiagnosticList(preview.Diagnostics);
            return;
        }

        ImGui::SeparatorText("Draft Interpretation");
        Typesetting::ImGuiTypesetLabelValue(
            typesettingService,
            "Normalized",
            MakeMathRequest(preview.NormalizedProjection, baseFontSize),
            inspectorTheme.SecondaryTextOptions,
            inspectorTheme.DetailMathOptions);
        if (preview.InferredKind.has_value()) {
            ImGui::BulletText("Inferred category: %s", ToString(*preview.InferredKind));
        }
        if (!preview.NormalizedInterpretation.empty()) {
            ImGui::TextWrapped("%s", preview.NormalizedInterpretation.c_str());
        }
        if (preview.SemanticDelta.has_value()) {
            DrawDraftSemanticDelta(*preview.SemanticDelta, navigateToDraftSemanticObject);
        }
        if (DrawReferencedSymbols(
                preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                preview.ReferencedSymbols,
                &preview.Assumptions,
                navigateToDraftSemanticObject,
                resolveDraftReferencedSymbol)) {
            return;
        }
        if (DrawPreviewDiagnosticSectionWithResolution(
                preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                MakeDefinitionObjectRefV2(preview.DefinitionId),
                preview.Diagnostics,
                preview.ReferencedSymbols,
                &preview.Assumptions,
                navigateToDraftSemanticObject,
                resolveDraftReferencedSymbol)) {
            return;
        }
        if (DrawDraftAssumptionWorkflowSummary(
            model,
            preview.Assumptions,
            preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
            navigateToDraftSemanticObject,
            acceptDraftAssumption)) {
            return;
        }
    } else if (editorBuffer->TargetKind == EModelObjectKindV2::Relation && editorBuffer->RelationPreview.has_value()) {
        const auto &preview = *editorBuffer->RelationPreview;
        if (!preview.bParseOk && preview.ParseError.has_value()) {
            DrawRawDiagnosticList(preview.Diagnostics);
            return;
        }

        ImGui::SeparatorText("Draft Interpretation");
        Typesetting::ImGuiTypesetLabelValue(
            typesettingService,
            "Normalized",
            MakeMathRequest(preview.NormalizedProjection, baseFontSize),
            inspectorTheme.SecondaryTextOptions,
            inspectorTheme.DetailMathOptions);
        ImGui::BulletText("Inferred category: %s", ToString(preview.InferredClass));
        if (!preview.NormalizedInterpretation.empty()) {
            ImGui::TextWrapped("%s", preview.NormalizedInterpretation.c_str());
        }
        if (preview.SemanticDelta.has_value()) {
            DrawDraftSemanticDelta(*preview.SemanticDelta, navigateToDraftSemanticObject);
        }
        if (DrawReferencedSymbols(
                preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                preview.ReferencedSymbols,
                &preview.Assumptions,
                navigateToDraftSemanticObject,
                resolveDraftReferencedSymbol)) {
            return;
        }
        if (DrawPreviewDiagnosticSectionWithResolution(
                preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
                MakeRelationObjectRefV2(preview.RelationId),
                preview.Diagnostics,
                preview.ReferencedSymbols,
                &preview.Assumptions,
                navigateToDraftSemanticObject,
                resolveDraftReferencedSymbol)) {
            return;
        }
        if (DrawDraftAssumptionWorkflowSummary(
            model,
            preview.Assumptions,
            preview.SemanticOverview.has_value() ? &(*preview.SemanticOverview) : nullptr,
            navigateToDraftSemanticObject,
            acceptDraftAssumption)) {
            return;
        }
    }
}

auto FLabV2WindowManager::DrawModelAssumptionsPanel() -> void {
    using namespace Slab::Core::Model::V2;

    auto state = PrepareModelWorkspaceViewState();
    if (!state.bAvailable || state.Model == nullptr) {
        ImGui::TextDisabled("No model state available.");
        return;
    }

    auto &model = *state.Model;
    auto &typesettingService = *UiTypesettingService;
    const auto baseFontSize = ImGui::GetFontSize();
    const auto &overview = state.Overview;
    const auto &semanticReport = overview.Report;

    const bool bHasDraftPreviewAssumptions =
        state.DraftPreviewAssumptions != nullptr && !state.DraftPreviewAssumptions->empty();
    if (semanticReport.Assumptions.empty() && !bHasDraftPreviewAssumptions) {
        ImGui::TextDisabled("No implicit or accepted semantics are currently available.");
        return;
    }

    if (bHasDraftPreviewAssumptions) {
        ImGui::SeparatorText("Draft Preview");
        for (const auto &assumption : *state.DraftPreviewAssumptions) {
            const auto ref = MakeAssumptionObjectRefV2(assumption.AssumptionId);
            const auto highlightState = ResolveHighlightState(state.SelectionContext, *state.SelectionOverview, ref);
            ImGui::PushID(("draft." + assumption.AssumptionId).c_str());
            const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                typesettingService,
                "##ModelDraftAssumptionRow",
                Typesetting::MakeTextRequest(
                    assumption.TargetSymbol.empty() ? assumption.Category : assumption.TargetSymbol,
                    Typesetting::FTypesetStyle{.FontPixelSize = baseFontSize * 0.95f}),
                MakeDraftAssumptionRowOptions(highlightState, baseFontSize));
            if (rowResult.bPressed) {
                SelectModelSemanticObject(model, ref, false, true);
            }
            ImGui::PopID();
        }
    }

    if (bHasDraftPreviewAssumptions && !semanticReport.Assumptions.empty()) {
        ImGui::SeparatorText("Canonical State");
    }
    for (const auto &assumption : semanticReport.Assumptions) {
        const auto ref = MakeAssumptionObjectRefV2(assumption.AssumptionId);
        const auto highlightState = ResolveHighlightState(state.SelectionContext, *state.SelectionOverview, ref);
        ImGui::PushID(assumption.AssumptionId.c_str());
        const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
            typesettingService,
            "##ModelAssumptionRow",
            Typesetting::MakeTextRequest(
                assumption.TargetSymbol.empty() ? assumption.Category : assumption.TargetSymbol,
                Typesetting::FTypesetStyle{.FontPixelSize = baseFontSize * 0.95f}),
            MakeAssumptionRowOptions(highlightState, baseFontSize));
        if (rowResult.bPressed) {
            SelectModelSemanticObject(model, ref, false, false);
        }
        ImGui::PopID();
    }
}

auto FLabV2WindowManager::DrawModelDetailsPanel() -> void {
    using namespace Slab::Core::Model::V2;

    auto state = PrepareModelWorkspaceViewState();
    if (!state.bAvailable || state.Model == nullptr) {
        ImGui::TextDisabled("No model state available.");
        return;
    }

    auto &model = *state.Model;
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

    const auto navigate = [&](const FSemanticObjectRefV2 &ref) {
        if (bSelectedModelSemanticObjectUsesDraftPreview) SelectModelSemanticObject(model, ref, true, true);
        else SelectModelSemanticObject(model, ref, true, false);
    };

    if (const auto *selectedObject = state.SelectionOverview->FindObject(SelectedModelSemanticObject); selectedObject != nullptr) {
        if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Definition) {
            const auto *definition = FindDefinitionByIdV2(model, SelectedModelSemanticObject.ObjectId);
            const auto *definitionReport = FindDefinitionReport(state.SelectionOverview->Report, SelectedModelSemanticObject.ObjectId);
            FModelEditorBufferV2 *editorBuffer = definition != nullptr
                ? EnsureEditorBuffer(ModelEditorBuffersByKey, model, EModelObjectKindV2::Definition, SelectedModelSemanticObject.ObjectId)
                : state.ActiveEditorBuffer;
            const auto *definitionPreview =
                editorBuffer != nullptr && editorBuffer->DefinitionPreview.has_value()
                    ? &(*editorBuffer->DefinitionPreview)
                    : nullptr;
            const auto draftDefinition = definition == nullptr && bSelectedModelSemanticObjectUsesDraftPreview
                ? FindDraftProposedDefinition(model, state.ActiveEditorBuffer, state.ActiveDraftOverview, SelectedModelSemanticObject.ObjectId)
                : Slab::TOptional<FDefinitionV2>{};
            if (definition != nullptr || draftDefinition.has_value()) {
                DrawDefinitionInspector(
                    typesettingService,
                    model,
                    definition != nullptr ? *definition : *draftDefinition,
                    *selectedObject,
                    definitionReport,
                    definitionPreview,
                    inspectorTheme,
                    navigate);
            }
        } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Relation) {
            const auto *relation = FindRelationByIdV2(model, SelectedModelSemanticObject.ObjectId);
            const auto *relationReport = FindRelationReport(state.SelectionOverview->Report, SelectedModelSemanticObject.ObjectId);
            FModelEditorBufferV2 *editorBuffer = EnsureEditorBuffer(
                ModelEditorBuffersByKey,
                model,
                EModelObjectKindV2::Relation,
                SelectedModelSemanticObject.ObjectId);
            const auto *relationPreview =
                editorBuffer != nullptr && editorBuffer->RelationPreview.has_value()
                    ? &(*editorBuffer->RelationPreview)
                    : nullptr;
            if (relation != nullptr) {
                DrawRelationInspector(
                    typesettingService,
                    model,
                    *relation,
                    *selectedObject,
                    relationReport,
                    relationPreview,
                    inspectorTheme,
                    navigate);
            }
        } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::VocabularyEntry) {
            if (const auto *entry = FindResolvedVocabularyEntry(state.Overview.VocabularyEntries, SelectedModelSemanticObject.ObjectId);
                entry != nullptr) {
                DrawVocabularyInspector(typesettingService, *entry, *selectedObject, inspectorTheme, navigate);
            }
        } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Assumption) {
            if (const auto *assumption = FindAssumptionReport(state.SelectionOverview->Report, SelectedModelSemanticObject.ObjectId);
                assumption != nullptr) {
                const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(model, *assumption, state.SelectionOverview);
                DrawAssumptionInspector(
                    typesettingService,
                    model,
                    *assumption,
                    *selectedObject,
                    materializationPreview,
                    inspectorTheme,
                    navigate);
            }
        }
    } else {
        ImGui::TextDisabled("Select a vocabulary entry, definition, relation, or assumption.");
    }
}
