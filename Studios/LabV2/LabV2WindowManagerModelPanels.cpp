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

    auto DrawReferencedSymbols(const Slab::Vector<ModelV2::FReferencedSymbolSemanticV2> &symbols) -> void {
        if (symbols.empty()) return;
        ImGui::SeparatorText("Referenced Symbols");
        for (const auto &symbol : symbols) {
            ImGui::PushID(symbol.SymbolText.c_str());
            ImGui::BulletText(
                "%s | %s",
                symbol.SymbolText.c_str(),
                symbol.bResolved ? "resolved" : "unresolved");
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
            if (!assumption.SourceId.empty()) {
                ImGui::TextDisabled("Source: %s", assumption.SourceId.c_str());
            }
            if (!assumption.Detail.empty()) {
                ImGui::TextWrapped("%s", assumption.Detail.c_str());
            }
            ImGui::PopID();
        }
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
                                 const ModelV2::FSemanticAssumptionV2 &assumption,
                                 const ModelV2::FSemanticObjectOverviewV2 &objectOverview,
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
        ModelPendingScrollTarget = {};
        SelectedModelVocabularyEntryId.clear();
        SelectedModelDefinitionId.clear();
        SelectedModelRelationId.clear();
        SelectedModelAssumptionId.clear();
        bSelectedModelDetailIsRelation = false;
        ModelEditorBuffersByKey.clear();
        ModelEditorStatus.clear();
        bModelHasLastChangeRecord = false;
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
        SelectedModelVocabularyEntryId.clear();
        SelectedModelAssumptionId.clear();
        ModelEditorBuffersByKey.clear();
        ModelEditorStatus = "[Ok] Switched base vocabulary to '" + model.BaseVocabulary.ActivePresetId + "'.";
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

    auto selectSemanticObject = [&](const FSemanticObjectRefV2 &ref, const bool bRequestScroll = false) {
        if (!ref.IsValid()) return;
        SelectedModelSemanticObject = ref;
        if (bRequestScroll) ModelPendingScrollTarget = ref;

        switch (ref.Kind) {
            case ESemanticObjectKindV2::Definition:
                SelectedModelDefinitionId = ref.ObjectId;
                bSelectedModelDetailIsRelation = false;
                break;
            case ESemanticObjectKindV2::Relation:
                SelectedModelRelationId = ref.ObjectId;
                bSelectedModelDetailIsRelation = true;
                break;
            case ESemanticObjectKindV2::VocabularyEntry:
                SelectedModelVocabularyEntryId = ref.ObjectId;
                break;
            case ESemanticObjectKindV2::Assumption:
                SelectedModelAssumptionId = ref.ObjectId;
                break;
        }
    };

    auto navigateToSemanticObject = [&](const FSemanticObjectRefV2 &ref) {
        selectSemanticObject(ref, true);
    };

    if (SelectedModelSemanticObject.IsValid() && overview.FindObject(SelectedModelSemanticObject) == nullptr) {
        SelectedModelSemanticObject = {};
    }
    if (!SelectedModelSemanticObject.IsValid()) {
        if (!SelectedModelDefinitionId.empty() && FindDefinitionByIdV2(model, SelectedModelDefinitionId) != nullptr) {
            selectSemanticObject(MakeDefinitionObjectRefV2(SelectedModelDefinitionId));
        } else if (!SelectedModelRelationId.empty() && FindRelationByIdV2(model, SelectedModelRelationId) != nullptr) {
            selectSemanticObject(MakeRelationObjectRefV2(SelectedModelRelationId));
        } else if (!SelectedModelVocabularyEntryId.empty()) {
            selectSemanticObject(MakeVocabularyObjectRefV2(SelectedModelVocabularyEntryId));
        } else if (!semanticReport.Assumptions.empty()) {
            selectSemanticObject(MakeAssumptionObjectRefV2(semanticReport.Assumptions.front().AssumptionId));
        }
    }
    if (!SelectedModelSemanticObject.IsValid() && !resolvedVocabularyEntries.empty()) {
        selectSemanticObject(MakeVocabularyObjectRefV2(resolvedVocabularyEntries.front().Entry.EntryId));
    }

    const auto selectionContext = BuildSemanticSelectionContextV2(overview, SelectedModelSemanticObject);

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

    DrawSummaryLinkStrip("State", overview.Status.CanonicalStateVariables, navigateToSemanticObject);
    DrawSummaryLinkStrip("Parameters", overview.Status.Parameters, navigateToSemanticObject);
    DrawSummaryLinkStrip("Fields", overview.Status.Fields, navigateToSemanticObject);
    DrawSummaryLinkStrip("Operators", overview.Status.Operators, navigateToSemanticObject);
    DrawSummaryLinkStrip("Observables", overview.Status.Observables, navigateToSemanticObject);

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
            ImGui::Dummy(ImVec2(0.0f, baseFontSize * 0.45f));

            const float vocabularyEntriesHeight = std::clamp(vocabularyPanelHeight * 0.48f, 84.0f, 152.0f);
            ImGui::BeginChild("ModelVocabularyEntriesList", ImVec2(0.0f, vocabularyEntriesHeight), false);
            for (const auto &entry : resolvedVocabularyEntries) {
                const auto ref = MakeVocabularyObjectRefV2(entry.Entry.EntryId);
                const auto highlightState = ResolveHighlightState(selectionContext, overview, ref);
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
            ImGui::Dummy(ImVec2(0.0f, baseFontSize * 0.35f));

            if (const auto *selectedVocabularyEntry = FindResolvedVocabularyEntry(resolvedVocabularyEntries, SelectedModelVocabularyEntryId);
                selectedVocabularyEntry != nullptr) {
                Typesetting::ImGuiTypesetMath(
                    typesettingService,
                    RenderBaseVocabularyEntryLatexV2(selectedVocabularyEntry->Entry),
                    inspectorTheme.DetailMathOptions);
                ImGui::TextDisabled(
                    "%s | %s",
                    ToString(selectedVocabularyEntry->Entry.Kind),
                    ToString(selectedVocabularyEntry->OverrideStatus));
                if (!selectedVocabularyEntry->Entry.SemanticRoleSummary.empty()) {
                    ImGui::TextWrapped("%s", selectedVocabularyEntry->Entry.SemanticRoleSummary.c_str());
                }
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
                const auto highlightState = ResolveHighlightState(selectionContext, overview, ref);
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
                const auto highlightState = ResolveHighlightState(selectionContext, overview, ref);
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
            if (const auto *selectedObject = overview.FindObject(SelectedModelSemanticObject); selectedObject != nullptr) {
                if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Definition) {
                    const auto *definition = FindDefinitionByIdV2(model, SelectedModelSemanticObject.ObjectId);
                    const auto *definitionReport = FindDefinitionReport(semanticReport, SelectedModelSemanticObject.ObjectId);
                    ModelV2::FModelEditorBufferV2 *editorBuffer = EnsureEditorBuffer(
                        ModelEditorBuffersByKey,
                        model,
                        EModelObjectKindV2::Definition,
                        SelectedModelSemanticObject.ObjectId);
                    const auto *definitionPreview =
                        editorBuffer != nullptr && editorBuffer->DefinitionPreview.has_value()
                            ? &(*editorBuffer->DefinitionPreview)
                            : nullptr;
                    if (definition != nullptr) {
                        DrawDefinitionInspector(
                            typesettingService,
                            model,
                            *definition,
                            *selectedObject,
                            definitionReport,
                            definitionPreview,
                            inspectorTheme,
                            navigateToSemanticObject);
                    }
                } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Relation) {
                    const auto *relation = FindRelationByIdV2(model, SelectedModelSemanticObject.ObjectId);
                    const auto *relationReport = FindRelationReport(semanticReport, SelectedModelSemanticObject.ObjectId);
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
                            navigateToSemanticObject);
                    }
                } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::VocabularyEntry) {
                    if (const auto *entry = FindResolvedVocabularyEntry(resolvedVocabularyEntries, SelectedModelSemanticObject.ObjectId);
                        entry != nullptr) {
                        DrawVocabularyInspector(
                            typesettingService,
                            *entry,
                            *selectedObject,
                            inspectorTheme,
                            navigateToSemanticObject);
                    }
                } else if (SelectedModelSemanticObject.Kind == ESemanticObjectKindV2::Assumption) {
                    if (const auto *assumption = FindAssumptionReport(semanticReport, SelectedModelSemanticObject.ObjectId);
                        assumption != nullptr) {
                        DrawAssumptionInspector(
                            typesettingService,
                            *assumption,
                            *selectedObject,
                            inspectorTheme,
                            navigateToSemanticObject);
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

        const auto editorTargetKind = bSelectedModelDetailIsRelation ? EModelObjectKindV2::Relation : EModelObjectKindV2::Definition;
        const auto editorTargetId = bSelectedModelDetailIsRelation ? SelectedModelRelationId : SelectedModelDefinitionId;
        auto *editorBuffer = EnsureEditorBuffer(ModelEditorBuffersByKey, model, editorTargetKind, editorTargetId);
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
            }

            if (ImGui::Button("Parse / Preview")) {
                if (ParseEditorBufferPreviewV2(model, *editorBuffer)) {
                    ModelEditorStatus = "[Ok] Parsed draft for " + editorBuffer->TargetId + ".";
                } else {
                    ModelEditorStatus = "[Error] Could not parse draft for " + editorBuffer->TargetId + ".";
                }
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
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(!editorBuffer->bDraftDirty && !editorBuffer->bPreviewCurrent);
            if (ImGui::Button("Revert / Cancel")) {
                if (RevertEditorBufferV2(model, *editorBuffer)) {
                    ModelEditorStatus = "[Ok] Restored canonical notation for " + editorBuffer->TargetId + ".";
                }
            }
            ImGui::EndDisabled();

            if (!editorBuffer->bPreviewCurrent) {
                ImGui::TextDisabled("Preview the current draft before applying.");
            } else if (editorBuffer->TargetKind == EModelObjectKindV2::Definition && editorBuffer->DefinitionPreview.has_value()) {
                const auto &preview = *editorBuffer->DefinitionPreview;
                if (!preview.bParseOk && preview.ParseError.has_value()) {
                    DrawRawDiagnosticList(preview.Diagnostics);
                } else {
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
                    DrawReferencedSymbols(preview.ReferencedSymbols);
                    DrawRawDiagnosticList(preview.Diagnostics);
                    DrawAssumptionSummary(preview.Assumptions);
                }
            } else if (editorBuffer->TargetKind == EModelObjectKindV2::Relation && editorBuffer->RelationPreview.has_value()) {
                const auto &preview = *editorBuffer->RelationPreview;
                if (!preview.bParseOk && preview.ParseError.has_value()) {
                    DrawRawDiagnosticList(preview.Diagnostics);
                } else {
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
                    DrawReferencedSymbols(preview.ReferencedSymbols);
                    DrawRawDiagnosticList(preview.Diagnostics);
                    DrawAssumptionSummary(preview.Assumptions);
                }
            }
        }
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("ModelAssumptionsPane", ImVec2(0.0f, 0.0f), true);
        ImGui::SeparatorText("Assumed / Inferred Semantics");
        if (semanticReport.Assumptions.empty()) {
            ImGui::TextDisabled("No implicit or accepted semantics are currently available.");
        } else {
            for (const auto &assumption : semanticReport.Assumptions) {
                const auto ref = MakeAssumptionObjectRefV2(assumption.AssumptionId);
                const auto highlightState = ResolveHighlightState(selectionContext, overview, ref);
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
                if (!assumption.Detail.empty()) {
                    ImGui::TextWrapped("%s", assumption.Detail.c_str());
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
                    if (ImGui::Button("Accept")) {
                        FModelChangeRecordV2 changeRecord;
                        if (AcceptAssumptionV2(model, assumption, &changeRecord)) {
                            ModelLastChangeRecord = changeRecord;
                            bModelHasLastChangeRecord = true;
                            ModelEditorStatus = "[Ok] Accepted semantic assumption '" + assumption.AssumptionId + "'.";
                            if (!changeRecord.ObjectId.empty() &&
                                changeRecord.ObjectKind == EModelObjectKindV2::Definition) {
                                selectSemanticObject(MakeDefinitionObjectRefV2(changeRecord.ObjectId), true);
                            } else {
                                selectSemanticObject(ref);
                            }
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
                    }
                }
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
